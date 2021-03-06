#include "GameManager.h"
#include <iostream>		//TODO: remove
#include <windows.h>	// WinApi header

bool GameManager::placePlayerPieces(GameBoard& board, PlayerAlgorithm& player, int playerNum) {
	vector<unique_ptr<PiecePosition>> vectorToFill;
	player.getInitialPositions(playerNum, vectorToFill);

	if (vectorToFill.empty()) {
		outFile << "Winner: " << getOppositePlayer(playerNum) << endl;
		outFile << "Reason: Bad Positioning input file for player " << playerNum << endl;
		cout << "Player " << playerNum << " lost. Positioned a piece out of board." << endl;
		return true;
	}

	map<PieceType, int> piecesCount = {
		{ Rock, ROCK_COUNT },
		{ Paper, PAPER_COUNT },
		{ Scissors, SCISSORS_COUNT },
		{ Bomb, BOMB_COUNT },
		{ Joker, JOKER_COUNT },
		{ Flag, FLAG_COUNT } };

	for (size_t i = 0; i < vectorToFill.size(); i++) {
		auto piecePos = vectorToFill[i].get();
		Position pos1 = piecePos->getPosition();
		Piece piece = Piece(getPieceType(piecePos->getPiece()), pos1, 1, piecePos->getPiece() == 'J');

		int otherPlayer = getOppositePlayer(playerNum);

		Position pos2 = piece.getPosition();
		if (!pos2.isInBoard()) {
			outFile << "Winner: " << otherPlayer << endl;
			outFile << "Reason: Bad Positioning input file for player " << playerNum << endl;
			cout << "Player " << playerNum << " lost. Positioned a piece out of board." << endl;
			return true;
		}

		PieceType type = piece.getIsJoker() ? Joker : piece.getType();
		piecesCount[type]--;
		if (piecesCount[type] < 0) {
			outFile << "Winner: " << otherPlayer << endl;
			outFile << "Reason: Bad Positioning input file for player " << playerNum << endl;
			cout << "Player " << playerNum << " lost. Positioned to many Pieces of type " << getPieceTypeRep(type) << "." << endl;
			return true;
		}

		Piece* toPosition;
		Position pos3 = piece.getPosition();

		if (type == Joker)
			toPosition = new Piece(getPieceType(vectorToFill[i].get()->getJokerRep()), pos3, playerNum, true);
		else
			toPosition = new Piece(type, pos3, playerNum, false);

		if (board[toPosition->getPosition()] != nullptr) {
			outFile << "Winner: " << otherPlayer << endl;
			outFile << "Reason: Bad Positioning input file for player " << playerNum << endl;
			cout << "Player " << playerNum << " lost. Positioned to pieces on the same position." << endl;
			return true;
		}

		board.setPos(vectorToFill[i].get()->getPosition(), toPosition);
	}

	return false;
}

void GameManager::gameVSgame(GameBoard& game1, GameBoard& game2, vector<unique_ptr<FightInfo>>& fights)
{
	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			Position pos = Position(j, i);
			Piece* piece1 = copyPiece(game1[pos]);
			Piece* piece2 = copyPiece(game2[pos]);
			Piece* winner = nullptr;

			if (piece1 == nullptr)
				winner = piece2;
			else if (piece2 == nullptr)
				winner = piece1;
			else
			{
				unique_ptr<Fight> fight;
				switch (piece1->getFightResult(piece2))
				{
				case FightWin:
					fight = make_unique<Fight>(pos, piece1->getRep(), piece2->getRep(), 1);
					delete(piece2);
					winner = piece1;
					break;
				case FightDraw:
					fight = make_unique<Fight>(pos, piece1->getRep(), piece2->getRep(), 0);
					delete(piece1);
					delete(piece2);
					winner = nullptr;
					break;
				case FightLose:
					fight = make_unique<Fight>(pos, piece1->getRep(), piece2->getRep(), 2);
					delete(piece1);
					winner = piece2;
					break;
				}

				fights.push_back(move(fight));
			}

			board.setPos(pos, winner);
		}
	}
}

bool GameManager::initBoard() {
	bool gameOver;

	GameBoard playerBoard1 = GameBoard();
	gameOver = placePlayerPieces(playerBoard1, player1, 1);
	if (gameOver)
		return true;

	GameBoard playerBoard2 = GameBoard();
	gameOver = placePlayerPieces(playerBoard2, player2, 2);
	if (gameOver)
		return true;

	vector<unique_ptr<FightInfo>> fights;
	gameVSgame(playerBoard1, playerBoard2, fights);
	player1.notifyOnInitialBoard(board, fights);
	player2.notifyOnInitialBoard(board, fights);

	return false;
}

pair<bool, bool> GameManager::applyMove(int playerNum) {
	PlayerAlgorithm& player = (playerNum == 1) ? player1 : player2;
	unique_ptr<Move> move = player.getMove();
	GameMove* realMove = (GameMove*)move.get();

	if (realMove == nullptr)
		return pair<bool, bool>(false, false);

	GameMessage message = board.move(*realMove);
	if (message.getMessage() != MoveOK) {
		outFile << "Winner: " << getOppositePlayer(playerNum) << endl;
		outFile << "Reason: Bad Moves input file for player " << playerNum << endl;
		cout << "Player " << playerNum << " lost. Bad move." << endl;
		return pair<bool, bool>(true, true);
	}

	PlayerAlgorithm& opponent = (playerNum == 1) ? player2 : player1;

	if (message.getFightInfo() == nullptr) {
		turnsSinceFight++;
		opponent.notifyOnOpponentMove(*realMove);
	}
	else {
		turnsSinceFight = 0;
		player.notifyFightResult(*message.getFightInfo());
		opponent.notifyOnOpponentMove(*realMove);
		opponent.notifyFightResult(*message.getFightInfo());
	}

	return pair<bool, bool>(false, true);;
}

bool GameManager::getJokerChange(int playerNum)
{
	PlayerAlgorithm& player = (playerNum == 1) ? player1 : player2;

	unique_ptr<JokerChange> jokerChange = player.getJokerChange();

	if (jokerChange.get() == nullptr) {
		return false;
	}
	else {
		Position pos = jokerChange.get()->getJokerChangePosition();
		char rep = jokerChange.get()->getJokerNewRep();
		bool validRep = (rep == 'B' || rep == 'S' || rep == 'R' || rep == 'P');
		if (!pos.isInBoard() || board[pos]->getPlayerNum() != playerNum || !validRep) {
			outFile << "Winner: " << getOppositePlayer(playerNum) << endl;
			outFile << "Reason: Bad Moves input file for player " << playerNum << endl;
			cout << "Player " << playerNum << " lost. Invalid joker change." << endl;
			return true;
		}

		board.changeJoker(pos, getPieceType(rep));
	}

	return false;
}

void PrintPretyBoard(GameBoard& board, HANDLE hConsole, PlayerAlgorithm& pl)
{
	//return;
	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			Position point = Position(j, i);
			Piece* p = ((AutoPlayerAlgorithm&)pl).opponentBoard.getPieceAt(point);
			if (p != nullptr)
			{
				bool flag = (p != nullptr && (p->getType() == Flag || p->getIsJoker()));
				SetConsoleTextAttribute(hConsole, (flag ? (BACKGROUND_INTENSITY | (board.getPlayer(point) + 10)) : board.getPlayer(point) + 10));

				//cout << getPieceTypeRep(((AlgoPiece*)p)->getType());
				if (((AlgoPiece*)p)->getIsKnown())
					cout << ((p != nullptr && p->getIsJoker()) ?
						getPieceTypeRep(p->getType()) :
						((AutoPlayerAlgorithm&)pl).opponentBoard.getPosRep(point));
				else
					cout << "?";

				continue;
			}

			p = board.getPieceAt(point);
			//Piece* p = board.getPieceAt(point);
			bool flag = (p != nullptr && (p->getType() == Flag || p->getIsJoker()));
			SetConsoleTextAttribute(hConsole, (flag ? (BACKGROUND_INTENSITY | (board.getPlayer(point) + 10)) : board.getPlayer(point) + 10));

			//cout << (p == nullptr ? "" : ("" + ((AlgoPiece*)p)->getFlagChance()));
			cout << ((p != nullptr && p->getIsJoker()) ?
				getPieceTypeRep(p->getType()) :
				board.getPosRep(point));
		}

		cout << '\n';
	}

	cout << endl << endl << endl;
	//cout << endl << "----------" << endl << endl;
}

void PrintPretyBoard(GameBoard& board, HANDLE hConsole)
{
	//return;
	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			Position point = Position(j, i);
			Piece* p = board.getPieceAt(point);
			//Piece* p = board.getPieceAt(point);
			bool flag = (p != nullptr && (p->getType() == Flag || p->getIsJoker()));
			SetConsoleTextAttribute(hConsole, (flag ? (BACKGROUND_INTENSITY | (board.getPlayer(point) + 10)) : board.getPlayer(point) + 10));

			cout << ((p != nullptr && p->getIsJoker()) ?
				getPieceTypeRep(p->getType()) :
				board.getPosRep(point));
		}

		cout << '\n';
	}

	cout << endl << endl << endl;
}

static int tiee = 0;
static int win1 = 0;
static int win2 = 0;

void GameManager::runGame()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	bool gameOver;
	bool hasLastPlayerMoved = true;

	gameOver = initBoard();
	if (gameOver)
		return;

	if (board.isGameOver()) {
		cout << "over" << endl;
		return;
	}

	while (!board.isGameOver()) {
		//getchar();

		PrintPretyBoard(board, hConsole);
		//PrintPretyBoard(board, hConsole, player1);

		pair<bool, bool> res = applyMove(board.getCurrentPlayer());
		gameOver = res.first;

		if (!hasLastPlayerMoved && res.second == false) { // both player didnt move so game is over
			outFile << "Winner: 0" << endl;
			outFile << "Reason: A tie - both Moves input files done without a winner" << endl;
			outFile << endl << board.getBoardRep();
			return;
		}
		else {
			hasLastPlayerMoved = res.second;
		}

		if (gameOver) {
			outFile << endl << board.getBoardRep();
			return;
		}

		if (turnsSinceFight == MAX_TURNS_SINCE_FIGHT) {
			tiee++;
			outFile << "Winner: 0" << endl;
			outFile << "Reason: Maximum amount of turns without fights reached" << endl; // TODO: make sure this is really what we need to print
			outFile << endl << board.getBoardRep();
			return;
		}

		if (board.isGameOver())
			break;

		gameOver = getJokerChange(board.getCurrentPlayer());
		if (gameOver) {
			outFile << endl << board.getBoardRep();
			return;
		}

		board.setCurrPlayer(board.getOtherPlayer());
	}

	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

	GameStatus status = board.getGameStatus();
	switch (status)
	{
	case StatusPlayer1FlagsCaptured:
		win2++;
		cout << "Winner: 2" << endl;
		cout << "Reason: All flags of the opponent are captured" << endl;
		break;
	case StatusPlayer2FlagsCaptured:
		win1++;
		cout << "Winner: 1" << endl;
		cout << "Reason: All flags of the opponent are captured" << endl;
		break;
	case StatusBothFlagsCaptured:
		tiee++;
		cout << "Winner: 0" << endl;
		cout << "Reason: A tie - all flags are eaten by both players in the position files" << endl;
		break;
	case StatusPlayer1NoMovingPieces:
		win2++;
		cout << "Winner: 2" << endl;
		cout << "Reason: All moving PIECEs of the opponent are eaten" << endl;
		break;
	case StatusPlayer2NoMovingPieces:
		win1++;
		cout << "Winner: 1" << endl;
		cout << "Reason: All moving PIECEs of the opponent are eaten" << endl;
		break;
	case StatusBothPlayersNoMovingPieces:
		tiee++;
		cout << "Winner: 0" << endl;
		cout << "Reason: A tie - all moving PIECEs are eaten" << endl;
		break;
	default:
		tiee++;
		cout << "Winner: 0" << endl;
		cout << "Reason: A tie - all moving PIECEs are eaten" << endl;
	}

	cout << "score: " << win1 << "-" << win2 << " (" << tiee << ")" << endl << endl;

	//GameStatus status = board.getGameStatus();
	//switch (status)
	//{
	//case StatusPlayer1FlagsCaptured:
	//	outFile << "Winner: 2" << endl;
	//	outFile << "Reason: All flags of the opponent are captured" << endl;
	//	break;
	//case StatusPlayer2FlagsCaptured:
	//	outFile << "Winner: 1" << endl;
	//	outFile << "Reason: All flags of the opponent are captured" << endl;
	//	break;
	//case StatusBothFlagsCaptured:
	//	outFile << "Winner: 0" << endl;
	//	outFile << "Reason: A tie - all flags are eaten by both players in the position files" << endl;
	//	break;
	//case StatusPlayer1NoMovingPieces:
	//	outFile << "Winner: 2" << endl;
	//	outFile << "Reason: All moving PIECEs of the opponent are eaten" << endl;
	//	break;
	//case StatusPlayer2NoMovingPieces:
	//	outFile << "Winner: 1" << endl;
	//	outFile << "Reason: All moving PIECEs of the opponent are eaten" << endl;
	//	break;
	//case StatusBothPlayersNoMovingPieces:
	//	outFile << "Winner: 0" << endl;
	//	outFile << "Reason: A tie - all moving PIECEs are eaten" << endl;
	//	break;
	//default:
	//	outFile << "Winner: 0" << endl;
	//	outFile << "Reason: A tie - all moving PIECEs are eaten" << endl;
	//}

	outFile << endl << board.getBoardRep();
	//cout << endl << board.getBoardRep();
	PrintPretyBoard(board, hConsole);

	//LOGFILE << "Just about to exit" << endl;

	return;
}
