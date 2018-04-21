#ifndef BOARD
#define BOARD

#include "Point.h"

class Board {

public:
	virtual int getPlayer(const Point& pos) const = 0;
};


#endif // !BOARD