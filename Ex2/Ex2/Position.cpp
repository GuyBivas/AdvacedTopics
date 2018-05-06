#include "Position.h"

bool operator==(const Point & p1, const Point & p2)
{
	return p1.getX() == p2.getX() && p1.getY() == p2.getY();
}
