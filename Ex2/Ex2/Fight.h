#ifndef FIGHT
#define FIGHT

#include "FightInfo.h"

class Fight : public FightInfo {
	Point& position;
	char opponentPiece;
	int winner;

public:
	Fight(Point& _position, char _opponentPiece, int _winner) : position(_position), opponentPiece(_opponentPiece), winner(_winner) { }

	virtual const Point& getPosition() const override { return position; }
	virtual char getOpponentPiece() const override { return opponentPiece; }
	virtual int getWinner() const override { return winner; }
	virtual ~Fight() {}
};

#endif // !FIGHT