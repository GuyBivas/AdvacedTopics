#ifndef FIGHT
#define FIGHT

#include "FightInfo.h"

class Fight : public FightInfo {
	Position position;
	char piece1;
	char piece2;
	int winner;

public:
	Fight(Point& _position, char _piece1, char _piece2, int _winner) : position(_position), piece1(_piece1), piece2(_piece2), winner(_winner) { }

	virtual const Point& getPosition() const override { return position; }
	virtual char getPiece(int player) const override { return player == 1 ? piece1 : piece2; }
	virtual int getWinner() const override { return winner; }
	virtual ~Fight() {}
};

#endif // !FIGHT