#ifndef JOKER_CHANGE
#define JOKER_CHANGE

#include "Point.h"

class JokerChange {

public:
	virtual const Point& getJokerChangePosition() const = 0;
	virtual char getJokerNewRep() const = 0; // R, P, S or B (but NOT J and NOT F)
};

#endif // !JOKER_CHANGE