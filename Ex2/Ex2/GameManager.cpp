#include "GameManager.h"





void placePlayerPieces(GameBoard& board, PlayerAlgorithm& player, int playerNum) {
	vector<unique_ptr<PiecePosition>> vectorToFill;
	player.getInitialPositions(playerNum, vectorToFill);
	for (size_t i = 0; i < vectorToFill.size(); i++) {
		PieceType type = getPieceType(vectorToFill[i].get()->getPiece());
		Point& pos = (Point&)(vectorToFill[i].get()->getPosition());
		Piece* piece;

		if (type == Joker)
			piece = new Piece(getPieceType(vectorToFill[i].get()->getJokerRep()), pos, playerNum, true);
		else
			piece = new Piece(type, pos, playerNum, false);

		board.setPos(vectorToFill[i].get()->getPosition(), piece);
	}
}

void GameManager::gameVSgame(GameBoard& game1, GameBoard& game2, vector<unique_ptr<FightInfo>>& fights1, vector<unique_ptr<FightInfo>>& fights2)
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
				unique_ptr<Fight> fight1;
				unique_ptr<Fight> fight2;
				switch (piece1->getFightResult(piece2))
				{
				case FightWin:
					fight1 = make_unique<Fight>(pos, piece2->getType(), 1);
					fight2 = make_unique<Fight>(pos, piece1->getType(), 1);
					delete(piece2);
					winner = piece1;
					break;
				case FightDraw:
					fight1 = make_unique<Fight>(pos, piece2->getType(), 0);
					fight2 = make_unique<Fight>(pos, piece1->getType(), 0);
					delete(piece1);
					delete(piece2);
					winner = NULL;
					break;
				case FightLose:
					fight1 = make_unique<Fight>(pos, piece2->getType(), 2);
					fight2 = make_unique<Fight>(pos, piece1->getType(), 2);
					delete(piece1);
					winner = piece2;
					break;
				}

				fights1.push_back(move(fight1));
				fights2.push_back(move(fight2));
			}

			board.setPos(pos, winner);
		}
	}
}

void GameManager::initBoard() {
	GameBoard playerBoard1 = GameBoard();
	placePlayerPieces(playerBoard1, player1, 1);
	GameBoard playerBoard2 = GameBoard();
	placePlayerPieces(playerBoard2, player2, 2);

	vector<unique_ptr<FightInfo>> fights1;
	vector<unique_ptr<FightInfo>> fights2;
	gameVSgame(playerBoard1, playerBoard2, fights1, fights2);
	player1.notifyOnInitialBoard(board, fights1);
	player2.notifyOnInitialBoard(board, fights2);
}

void GameManager::applyMove(PlayerAlgorithm& player, int playerNum) {

	unique_ptr<Move> move = player.getMove();
	
}

void GameManager::runGame() {
	initBoard();
	//TODO: this
}
