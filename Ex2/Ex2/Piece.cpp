#include "Piece.h"

Piece* copyPiece(Piece* piece)
{
	if (piece == NULL)
		return NULL;

	return new Piece(*piece);
}

FightResult Piece::getFightResult(const Piece* other) const
{
	if (other == NULL)
		return FightWin;

	if (other->type == type)
		return FightDraw;

	if (other->type == Bomb)
		return FightDraw;

	if (other->type == Flag)
		return FightWin;

	if ((type == Rock && other->type == Scissors) ||
		(type == Paper && other->type == Rock) ||
		(type == Scissors && other->type == Paper))
	{
		return FightWin;
	}
	else
	{
		return FightLose;
	}
}

char Piece::getRep() const
{
	char res;

	switch (type)
	{
	case Rock:
		res = 'R';
		break;
	case Paper:
		res = 'P';
		break;
	case Scissors:
		res = 'S';
		break;
	case Bomb:
		res = 'B';
		break;
	case Flag:
		res = 'F';
		break;
	default:
		res = '0';
		break;
	}

	if (isJoker)
		res = 'J';

	if (playerNum == 2)
		res += 'a' - 'A';

	return res;
}