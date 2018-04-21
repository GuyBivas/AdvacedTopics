#ifndef MOVE
#define MOVE

#include "Point.h"

class Move {

public:
	virtual const Point& getFrom() const = 0;
	virtual const Point& getTo() const = 0;
};

#endif // !MOVE
