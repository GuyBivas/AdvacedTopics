#include "Piece.h"

Piece* copyPiece(Piece* piece)
{
	if (piece == NULL)
		return NULL;

	return new Piece(*piece);
}

char getPieceTypeRep(PieceType type)
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

	return res;
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


// TODO: check if need to return capitalized or not (dependent on the player)
char Piece::getPiece() const {
	char res = getPieceTypeRep(type);

	if (isJoker)
		res = 'J';

	if (playerNum == 2)
		res += 'a' - 'A';

	return res;
}

char Piece::getJokerRep() const {
	return getPieceTypeRep(type);
}

