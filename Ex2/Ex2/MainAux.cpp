#include "MainAux.h"

using namespace std;

tuple<ParserMessageType, int> placePlayerPieces(GameBoard* game, int playerNum, ifstream& infile)
{
	map<PieceType, int> piecesCount = {
	{ Rock, ROCK_COUNT },
	{ Paper, PAPER_COUNT },
	{ Scissors, SCISSORS_COUNT },
	{ Bomb, BOMB_COUNT },
	{ Joker, JOKER_COUNT },
	{ Flag, FLAG_COUNT } };

	string line;
	int lineNumber = 1;


	while (getline(infile, line))
	{
		PositioningCommand command = parsePositioningCommand(line);

		if (command.getMessageType() != ParseOK)
			return tuple<ParserMessageType, int>(command.getMessageType(), lineNumber);

		if ((*game)[command.getPos()] != NULL)
			return tuple<ParserMessageType, int>(ParseMultiplePiecesInPos, lineNumber);

		if (command.getIsJoker())
		{
			piecesCount[Joker]--;
			if (piecesCount[Joker] < 0)
				return tuple<ParserMessageType, int>(ParsePositionedTooManyPieces, lineNumber);
		}
		else
		{
			PieceType piece = command.getType();
			piecesCount[piece]--;
			if (piecesCount[piece] < 0)
				return tuple<ParserMessageType, int>(ParsePositionedTooManyPieces, lineNumber);
		}

		game->setPos(command.getPos(), new Piece(command.getType(), command.getPos(), playerNum, command.getIsJoker()));

		lineNumber++;
	}

	if (piecesCount[Flag] != 0)
		return tuple<ParserMessageType, int>(ParseNotPositionedAllFlags, lineNumber - 1);

	return tuple<ParserMessageType, int>(ParseOK, 0);
}

GameBoard* gameVSgame(GameBoard& game1, GameBoard& game2)
{
	GameBoard* result = new GameBoard();

	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			Position pos = Position(j, i);
			Piece* piece1 = copyPiece(game1[pos]);
			Piece* piece2 = copyPiece(game2[pos]);
			Piece* winner = NULL;

			if (piece1 == NULL)
				winner = piece2;
			else
			{
				switch (piece1->getFightResult(piece2))
				{
				case FightWin:
					delete(piece2);
					winner = piece1;
					break;
				case FightDraw:
					delete(piece1);
					delete(piece2);
					winner = NULL;
					break;
				case FightLose:
					delete(piece1);
					winner = piece2;
					break;
				}
			}

			result->setPos(pos, winner);
		}
	}

	return result;
}

// aplies the part of the move that moves the piece. In case of an error, prints the 
// relevant messages to stdout and output file.
// retuns true in case of game over, false otherwise.
bool movePlayerPiece(GameBoard& game, MoveCommand moveCommand, int lineNumber, ofstream& outFile)
{
	ParserMessageType messageType = moveCommand.getMessageType();

	if (messageType == ParseOK)
	{
		GameMessage message = game.move(moveCommand.getMove());
		switch (message.getMessage())
		{
		case MoveOK:
			return false;
		case BadMoveFrom:
			cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". From position does not contain player's piece." << endl;
			break;
		case BadMoveTo:
			cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". To position contains a player's piece." << endl;
			break;
		case BadMovement:
			cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". Movement is invalid." << endl;
			break;
		case NoJokerInPos:
			cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". Position of supposed joker does not contain a player's joker." << endl;
			break;
		default:
			break;
		}
		char buff[128];
		sprintf(buff, "Bad Moves input file for player %d - line %d", game.getCurrentPlayer(), lineNumber);
		outFile << "Winner: " << game.getOtherPlayer() << endl;
		outFile << "Reason: " << buff << endl;
		return true;
	}
	else if (messageType == ParseOutOfBounds)
	{
		cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". Positions out of bound." << endl;
		char buff[128];
		sprintf(buff, "Bad Moves input file for player %d - line %d", game.getCurrentPlayer(), lineNumber);
		outFile << "Winner: " << game.getOtherPlayer() << endl;
		outFile << "Reason: " << buff << endl;
		return true;
	}
	else
	{
		cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". Invalid line format." << endl;
		char buff[128];
		sprintf(buff, "Bad Moves input file for player %d - line %d", game.getCurrentPlayer(), lineNumber);
		outFile << "Winner: " << game.getOtherPlayer() << endl;
		outFile << "Reason: " << buff << endl;
		return true;
	}
}

// aplies the part of the move that transforms the joker. In case of an error, prints the 
// relevant messages to stdout and output file.
// retuns true in case of game over, false otherwise.
bool transformJokerMove(GameBoard& game, MoveCommand moveCommand, int lineNumber, ofstream& outFile)
{
	GameMessage message = game.transformJoker(moveCommand.getJokerPos(), moveCommand.getJokerNewRep());
	GameMessageType messageType = message.getMessage();
	switch (messageType)
	{
	case MoveOK:
		return false;
	case NoJokerInPos:
		cout << "Bad Moves input file for player " << game.getCurrentPlayer() << " - line " << lineNumber << ". Position of supposed joker does not contain a player's joker." << endl;
		char buff[128];
		sprintf(buff, "Bad Moves input file for player %d - line %d", game.getCurrentPlayer(), lineNumber);
		outFile << "Winner: " << game.getOtherPlayer() << endl;
		outFile << "Reason: " << buff << endl;
		break;
	default:
		break;
	}
	return true;
}

// checks the status of the game. In case the game is over, prints the relevant
// messages to output file and stdout.
// returns true if game is over, false otherwise
bool checkStatus(GameBoard& game, ofstream& outFile)
{
	GameStatus status = game.getGameStatus();
	string message;
	int winner;
	switch (status)
	{
	case StatusNormal:
		return false;
	case StatusPlayer1FlagsCaptured:
		message = "All flags of the opponent are captured";
		winner = 2;
		break;
	case StatusPlayer2FlagsCaptured:
		message = "All flags of the opponent are captured";
		winner = 1;
		break;
	case StatusBothFlagsCaptured:
		break;
	case StatusPlayer1NoMovingPieces:
		message = "All moving PIECEs of the opponent are eaten";
		winner = 2;
		break;
	case StatusPlayer2NoMovingPieces:
		message = "All moving PIECEs of the opponent are eaten";
		winner = 1;
		break;
	case StatusBothPlayersNoMovingPieces:
		message = "A tie - all moving PIECEs are eaten";
		winner = 0;
		break;
	default:
		break;
	}

	cout << "Winner: " << winner << endl;
	cout << "Reason: " << message << endl;

	outFile << "Winner: " << winner << endl;
	outFile << "Reason: " << message << endl;
	return true;
}

bool applyMoveCommand(GameBoard& game, string line, int lineNumber, ofstream& outFile)
{
	MoveCommand moveCommand = parseMoveCommand(line);
	bool gameEnded = movePlayerPiece(game, moveCommand, lineNumber, outFile); // move the piece
	if (!gameEnded)
	{
		gameEnded = checkStatus(game, outFile);
		if (!gameEnded)
		{
			gameEnded = transformJokerMove(game, moveCommand, lineNumber, outFile); // transform the joker
			if (!gameEnded)
				return checkStatus(game, outFile);
		}
	}

	return true;
}

bool applyAllMoves(GameBoard& game, ifstream& movesFile1, ifstream& movesFile2, ofstream& outFile)
{
	bool hasMovesPlayer1 = true;
	bool hasMovesPlayer2 = true;

	string line;

	int linePlayer1 = 1;
	int linePlayer2 = 1;

	bool gameEnded = false;

	while ((hasMovesPlayer1 || hasMovesPlayer2) && !gameEnded)
	{
		if (hasMovesPlayer1 && !gameEnded)
		{
			if (getline(movesFile1, line))
			{
				game.setCurrPlayer(1);
				gameEnded = applyMoveCommand(game, line, linePlayer1, outFile);
				linePlayer1++;
			}
			else
			{
				hasMovesPlayer1 = false;
			}
		}
		if (hasMovesPlayer2 && !gameEnded)
		{
			if (getline(movesFile2, line))
			{
				game.setCurrPlayer(2);
				gameEnded = applyMoveCommand(game, line, linePlayer2, outFile);
				linePlayer2++;
			}
			else
			{
				hasMovesPlayer2 = false;
			}
		}
	}

	return gameEnded;
}

void printPlayerPositioningResult(tuple<ParserMessageType, int> tuple, int playerNum)
{
	ParserMessageType message = get<0>(tuple);
	int lineNum = get<1>(tuple);

	switch (message)
	{
	case ParseOK:
		break;
	case ParseInvalidCommand:
		printf("Bad Positioning input file for player %d - line %d. Invalid line format.\n", playerNum, lineNum);
		break;
	case ParseOutOfBounds:
		printf("Bad Positioning input file for player %d - line %d. Position out of board.\n", playerNum, lineNum);
		break;
	case ParseMultiplePiecesInPos:
		printf("Bad Positioning input file for player %d - line %d. Positioned two pieces in the same position.\n", playerNum, lineNum);
		break;
	case ParseInvalidJokerRep:
		printf("Bad Positioning input file for player %d - line %d. Invalid joker representation.\n", playerNum, lineNum);
		break;
	case ParsePositionedTooManyPieces:
		printf("Bad Positioning input file for player %d - line %d. Positioned too many pieces.\n", playerNum, lineNum);
		break;
	case ParseNotPositionedAllFlags:
		printf("Bad Positioning input file for player %d - line %d. Not positioned all flags.\n", playerNum, lineNum);
		break;
	default:
		break;
	}
}

bool printPositioningResult(tuple<ParserMessageType, int> tuple1, tuple<ParserMessageType, int> tuple2, ofstream& outFile)
{
	printPlayerPositioningResult(tuple1, 1);
	printPlayerPositioningResult(tuple2, 2);

	ParserMessageType message1 = get<0>(tuple1);
	ParserMessageType message2 = get<0>(tuple2);
	int lineNumber1 = get<1>(tuple1);
	int lineNumber2 = get<1>(tuple2);
	char buff[200];

	if (message1 == ParseOK && message2 == ParseOK)
	{
		return false;
	}
	else if (message1 != ParseOK && message2 == ParseOK)
	{
		sprintf(buff, "Bad Positioning input file for player 1 - line %d", lineNumber1);
		cout << "Winner: 2" << endl;
		cout << "Reason: " << string(buff) << endl;
		outFile << "Winner: 2" << endl;
		outFile << "Reason: " << string(buff) << endl;
		return true;
	}
	else if (message1 == ParseOK && message2 != ParseOK)
	{
		sprintf(buff, "Bad Positioning input file for player 2 - line %d", lineNumber2);
		cout << "Winner: 1" << endl;
		cout << "Reason: " << string(buff) << endl;
		outFile << "Winner: 1" << endl;
		outFile << "Reason: " << string(buff) << endl;
		return true;
	}
	else // if (message1 != ParseOK && message2 != ParseOK)
	{
		sprintf(buff, "Bad Positioning input file for both players - player 1: line %d, player 2: line %d", lineNumber1, lineNumber2);
		cout << "Winner: 0" << endl;
		cout << "Reason: " << string(buff) << endl;
		outFile << "Winner: 0" << endl;
		outFile << "Reason: " << string(buff) << endl;
		return true;
	}
}

bool printGameVsGameResult(GameStatus status, ofstream& outFile)
{
	string result;
	int winner = 0;

	switch (status)
	{
	case StatusNormal:
		return false;
	case StatusBothFlagsCaptured:
		result = "A tie - all flags are eaten by both players in the position files";
		break;
	case StatusPlayer1FlagsCaptured:
		result = "All flags of the opponent are captured";
		winner = 2;
		break;
	case StatusPlayer2FlagsCaptured:
		result = "All flags of the opponent are captured";
		winner = 1;
		break;
	case StatusPlayer1NoMovingPieces:
		result = "All moving PIECEs of the opponent are eaten";
		winner = 2;
		break;
	case StatusPlayer2NoMovingPieces:
		result = "All moving PIECEs of the opponent are eaten";
		winner = 1;
		break;
	case StatusBothPlayersNoMovingPieces:
		result = "A tie - all moving pieces are eaten by both players in the position files";
		break;
	default:
		break;
	}

	cout << "Winner: " << winner << endl;
	cout << "Reason: " << result << endl;

	outFile << "Winner: " << winner << endl;
	outFile << "Reason: " << result << endl;

	return true;
}