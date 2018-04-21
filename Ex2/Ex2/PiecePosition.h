#ifndef PIECE_POSITION
#define PIECE_POSITION

#include "Point.h"

class PiecePosition {

public:
	virtual const Point& getPosition() const = 0;
	virtual char getPiece() const = 0;
	virtual char getJokerRep() const = 0;
};

#endif // !PIECE_POSITION
