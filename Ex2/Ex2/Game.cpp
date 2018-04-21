#include "Game.h"

string Game::getBoardRep()
{
	char res[ROWS * (COLS + 1) + 1];
	int index = 0;

	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			res[index++] = getPosRep(Position(j, i));
		}

		res[index++] = '\n';
	}

	res[index] = '\0';

	return string(res);
}

void Game::movePiece(Position const& from, Position const& to)
{
	delete(getPieceAt(to));

	Piece* fromPiece = getPieceAt(from);
	fromPiece->setPos(to);
	setPos(to, fromPiece);
	setPos(from, NULL);
}

GameMessage Game::changeJoker(Position jokerPos, PieceType newType)
{
	Piece* joker = getPieceAt(jokerPos);
	if (joker == NULL || joker->getIsJoker() == false || joker->getPlayerNum() != currentPlayer)
		return GameMessage(NoJokerInPos, currentPlayer);

	joker->setType(newType);
	return GameMessage(MoveOK, currentPlayer);
}

int Game::getOtherPlayer() const
{
	if (getCurrentPlayer() == 1)
		return 2;
	else
		return 1;
}

GameMessage Game::isValidMove(Position const& from, Position const& to)
{
	if (!containsCurrPlayerPiece(from))
		return GameMessage(BadMoveFrom, currentPlayer);

	if (containsCurrPlayerPiece(to))
		return GameMessage(BadMoveTo, currentPlayer);

	if ((to - from).magnitude() != 1 ||
		getPieceAt(from)->getType() == Flag || getPieceAt(from)->getType() == Bomb)
		return GameMessage(BadMovement, currentPlayer);

	return GameMessage(MoveOK, currentPlayer);
}

//GameMessage Game::move(Position const& from, Position const& to, Position const& jokerPos, PieceType jokerNewType)
GameMessage Game::move(Position const& from, Position const& to)
{
	GameMessage isValidMessage = isValidMove(from, to);
	if (isValidMessage.getMessage() != MoveOK)
		return isValidMessage;

	Piece* fromPiece = getPieceAt(from);
	Piece* toPiece = getPieceAt(to);
	

	FightResult res = fromPiece->getFightResult(toPiece);
	switch (res)
	{
	case FightWin:
		movePiece(from, to);
		break;
	case FightDraw:
		delete(getPieceAt(from));
		delete(getPieceAt(to));
		setPos(from, NULL);
		setPos(to, NULL);
		break;
	case FightLose:
		delete(getPieceAt(from));
		setPos(from, NULL);
		break;
	}

	return GameMessage(MoveOK, currentPlayer);
}


GameMessage Game::transformJoker(Position const& jokerPos, PieceType jokerNewType) 
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

GameStatus Game::getGameStatus()
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

Game::Game()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			board[i][j] = NULL;
		}
	}
}

Game::~Game()
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete(board[row][col]);
		}
	}
}