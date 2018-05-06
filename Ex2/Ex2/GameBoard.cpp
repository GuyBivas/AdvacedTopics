#include "GameBoard.h"

string GameBoard::getBoardRep()
{
	char res[ROWS * (COLS + 1) + 1];
	int index = 0;

	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			Position point = Position(j, i);
			res[index++] = getPosRep(point);
		}

		res[index++] = '\n';
	}

	res[index] = '\0';

	return string(res);
}

void GameBoard::movePiece(const GameMove& move)
{
	delete(getPieceAt(move.getTo()));

	Piece* fromPiece = getPieceAt(move.getFrom());
	fromPiece->setPos(move.getTo());
	setPos(move.getTo(), fromPiece);
	setPos(move.getFrom(), NULL);
}

GameMessage GameBoard::changeJoker(Point& jokerPos, PieceType newType)
{
	Piece* joker = getPieceAt(jokerPos);
	if (joker == NULL || joker->getIsJoker() == false || joker->getPlayerNum() != currentPlayer)
		return GameMessage(NoJokerInPos, currentPlayer);

	joker->setType(newType);
	return GameMessage(MoveOK, currentPlayer);
}

int getOppositePlayer(int player)
{
	return (player == 1) ? 2 : 1;
}

bool GameBoard::isGameOver() const
{
	return (getGameStatus() != StatusNormal);
}

int GameBoard::getOtherPlayer() const
{
	return getOppositePlayer(currentPlayer);
}

GameMessage GameBoard::isValidMove(const GameMove& move)
{
	if (!containsCurrPlayerPiece(move.getFrom()))
		return GameMessage(BadMoveFrom, currentPlayer);

	if (containsCurrPlayerPiece(move.getTo()))
		return GameMessage(BadMoveTo, currentPlayer);

	if (move.diff().magnitude() != 1 ||
		getPieceAt(move.getFrom())->getType() == Flag || getPieceAt(move.getFrom())->getType() == Bomb)
		return GameMessage(BadMovement, currentPlayer);

	return GameMessage(MoveOK, currentPlayer);
}

//GameMessage Game::move(Point const& from, Point const& to, Point const& jokerPos, PieceType jokerNewType)
GameMessage GameBoard::move(const GameMove& move)
{
	GameMessage isValidMessage = isValidMove(move);
	if (isValidMessage.getMessage() != MoveOK)
		return isValidMessage;

	Piece* fromPiece = getPieceAt(move.getFrom());
	Piece* toPiece = getPieceAt(move.getTo());

	FightResult res = fromPiece->getFightResult(toPiece);
	Fight* fightInfo = nullptr;
	if (toPiece != NULL) {
		int currPlayer = getCurrentPlayer();
		char piece1 = (fromPiece->getPlayerNum() == 1) ? fromPiece->getRep() : toPiece->getRep();
		char piece2 = (fromPiece->getPlayerNum() == 2) ? fromPiece->getRep() : toPiece->getRep();
		int winner;
		if (res == FightDraw)
			winner = 0;
		else if (res == FightWin) {
			winner = currPlayer;
		}
		else {
			winner = getOppositePlayer(currPlayer);
		}
		fightInfo = new Fight((Point&)move.getTo(), piece1, piece2, winner);
	}

	switch (res)
	{
	case FightWin:
		movePiece(move);
		break;
	case FightDraw:
		delete(getPieceAt(move.getFrom()));
		delete(getPieceAt(move.getTo()));
		setPos(move.getFrom(), NULL);
		setPos(move.getTo(), NULL);
		break;
	case FightLose:
		delete(getPieceAt(move.getFrom()));
		setPos(move.getFrom(), NULL);
		break;
	}

	return GameMessage(MoveOK, currentPlayer);
}


GameMessage GameBoard::transformJoker(Point const&jokerPos, PieceType jokerNewType)
{
	Piece* joker = getPieceAt(jokerPos);
	if (jokerNewType != -1)
	{
		if (joker == NULL || !joker->getIsJoker())
			return GameMessage(NoJokerInPos, currentPlayer);
		joker->setType(jokerNewType);
	}
	return GameMessage(MoveOK, currentPlayer);
}

GameStatus GameBoard::getGameStatus() const
{
	bool player1MovingPieces = false;
	bool player2MovingPieces = false;
	bool player1Flags = false;
	bool player2Flags = false;

	for (int row = 1; row <= ROWS; row++)
	{
		for (int col = 1; col <= COLS; col++)
		{
			Piece* piece = getPieceAt(Position(row, col));
			if (piece == NULL)
				continue;

			int playerNum = piece->getPlayerNum();

			switch (piece->getType())
			{
			case Rock:
			case Paper:
			case Scissors:
				if (playerNum == 1)
					player1MovingPieces = true;
				else
					player2MovingPieces = true;
				break;
			case Flag:
				if (playerNum == 1)
					player1Flags = true;
				else
					player2Flags = true;
				break;
			default:
				break;
			}
		}
	}

	if (!player1Flags && !player2Flags)
		return StatusBothFlagsCaptured;
	else if (!player1Flags)
		return StatusPlayer1FlagsCaptured;
	else if (!player2Flags)
		return StatusPlayer2FlagsCaptured;

	else if (!player1MovingPieces && !player2MovingPieces)
		return StatusBothPlayersNoMovingPieces;
	else if (!player1MovingPieces)
		return StatusPlayer1NoMovingPieces;
	else if (!player2MovingPieces)
		return StatusPlayer2NoMovingPieces;
	else
		return StatusNormal;

}

GameBoard::GameBoard()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			board[i][j] = NULL;
		}
	}
}

GameBoard::~GameBoard()
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete(board[row][col]);
		}
	}
}

int GameBoard::getPlayer(const Point& pos) const {
	Piece* piece = getPieceAt(pos);
	if (piece == NULL)
		return 0;
	else
		return piece->getPlayerNum();
}
