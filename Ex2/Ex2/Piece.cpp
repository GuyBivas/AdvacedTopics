#include "Piece.h"

Piece* copyPiece(Piece* piece)
{
	if (piece == nullptr)
		return nullptr;

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
	if (other == nullptr)
		return FightWin;

	if (other->type == type)
		return FightDraw;

	if (type == Bomb || other->type == Bomb)
		return FightDraw;

	if (other->type == Flag)
		return FightWin;

	if (type == Flag)
		return FightLose;

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

PieceType getPieceType(char c)
{
	if ('a' <= c && c <= 'z')
		c = c + 'A' - 'a';
	
	switch (c)
	{
	case 'R':
		return Rock;
	case 'P':
		return Paper;
	case 'S':
		return Scissors;
	case 'B':
		return Bomb;
	case 'F':
		return Flag;
	case 'J':
		return Joker;
	default:
		return (PieceType)-1;
	}
}