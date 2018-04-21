#ifndef FIGHT
#define FIGHT

#include "FightInfo.h"

class Fight : public FightInfo {
	
public:
	virtual const Point& getPosition() const override
	virtual char getOpponentPiece() const = 0;
	virtual int getWinner() const = 0;
};

#endif // !FIGHT
