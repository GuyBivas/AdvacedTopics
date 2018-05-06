#include "GameManager.h"



bool GameManager::placePlayerPieces(GameBoard& board, PlayerAlgorithm& player, int playerNum) {
	vector<unique_ptr<PiecePosition>> vectorToFill;
	player.getInitialPositions(playerNum, vectorToFill);

	map<PieceType, int> piecesCount = {
		{ Rock, ROCK_COUNT },
		{ Paper, PAPER_COUNT },
		{ Scissors, SCISSORS_COUNT },
		{ Bomb, BOMB_COUNT },
		{ Joker, JOKER_COUNT },
		{ Flag, FLAG_COUNT } };

	for (size_t i = 0; i < vectorToFill.size(); i++) {
		auto piecePos = vectorToFill[i].get();
		Piece piece(getPieceType(piecePos->getPiece()), (Position&)piecePos->getPosition(), 1, piecePos->getPiece() == 'J');

		int otherPlayer = getOppositePlayer(playerNum);
		
		if (!((Position&)piece.getPosition()).isInBoard()) {
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

		if (type == Joker)
			toPosition = new Piece(getPieceType(vectorToFill[i].get()->getJokerRep()), (Position&)piece.getPosition(), playerNum, true);
		else
			toPosition = new Piece(type, (Position&)piece.getPosition(), playerNum, false);

		if (board[toPosition->getPosition()] != NULL) {
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
			Piece* winner = NULL;

			if (piece1 == NULL)
				winner = piece2;
			else if (piece2 == NULL)
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
					winner = NULL;
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

bool GameManager::applyMove(int playerNum) {
	PlayerAlgorithm& player = (playerNum == 1) ? player1 : player2;
	unique_ptr<Move> move = player.getMove();
	GameMove* realMove = (GameMove*)move.get();
	
	GameMessage message = board.move(*realMove);
	if (message.getMessage() != MoveOK) {
		outFile << "Winner: " << getOppositePlayer(playerNum) << endl;
		outFile << "Reason: Bad Moves input file for player " << playerNum << endl;
		cout << "Player " << playerNum << " lost. Bad move." << endl;
		return true;
	}

	PlayerAlgorithm& opponent = (playerNum == 1) ? player2 : player1;

	if (message.getFightInfo() == nullptr)
		turnsSinceFight++;
	else
		turnsSinceFight = 0;

	player.notifyFightResult(*message.getFightInfo());
	
	opponent.notifyOnOpponentMove(*realMove);
	opponent.notifyFightResult(*message.getFightInfo());

	return false;
}

bool GameManager::getJokerChange(int playerNum)
{
	PlayerAlgorithm& player = (playerNum == 1) ? player1 : player2;

	unique_ptr<JokerChange> jokerChange = player.getJokerChange();
	
	if (jokerChange.get() == nullptr)
		return false;
	
	else {
		Position pos = (Position&)jokerChange.get()->getJokerChangePosition();
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

void GameManager::runGame() {
	
	bool gameOver;

	gameOver = initBoard();
	if (gameOver)
		return;

	while (!board.isGameOver()) {
		gameOver = applyMove(board.getCurrentPlayer());
		if (gameOver) {
			outFile << endl << board.getBoardRep();
			return;
		}

		if (turnsSinceFight == MAX_TURNS_SINCE_FIGHT) {
			outFile << "Winner: 0" << endl;
			outFile << "Reason: Maximum amount of turns without fights reached" << endl; // TODO: make sure this is really what we need to print
			outFile << endl << board.getBoardRep();
		}
		
		gameOver = getJokerChange(board.getCurrentPlayer());
		if (gameOver) {
			outFile << endl << board.getBoardRep();
			return;
		}

		board.setCurrPlayer(board.getOtherPlayer());
	}

	GameStatus status = board.getGameStatus();
	
	switch (status)
	{
	case StatusPlayer1FlagsCaptured:
		outFile << "Winner: 2" << endl;
		outFile << "Reason: All flags of the opponent are captured" << endl;
	case StatusPlayer2FlagsCaptured:
		outFile << "Winner: 1" << endl;
		outFile << "Reason: All flags of the opponent are captured" << endl;
	case StatusBothFlagsCaptured:
		outFile << "Winner: 0" << endl;
		outFile << "Reason: A tie - all flags are eaten by both players in the position files" << endl;
	case StatusPlayer1NoMovingPieces:
		outFile << "Winner: 2" << endl;
		outFile << "Reason: All moving PIECEs of the opponent are eaten" << endl;
	case StatusPlayer2NoMovingPieces:
		outFile << "Winner: 1" << endl;
		outFile << "Reason: All moving PIECEs of the opponent are eaten" << endl;
	case StatusBothPlayersNoMovingPieces:
		outFile << "Winner: 0" << endl;
		outFile << "Reason: A tie - all moving PIECEs are eaten" << endl;
	default:
		outFile << "Winner: 0" << endl;
		outFile << "Reason: A tie - all moving PIECEs are eaten" << endl;
	}
	outFile << endl << board.getBoardRep();

	return;
}
