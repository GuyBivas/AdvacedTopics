#ifndef JOKER_TRANSFORM
#define JOKER_TRANSFORM

#include "JokerChange.h"
#include "Position.h"
#include "Piece.h"

class JokerTransform : public JokerChange {

private:
	Position pos;
	PieceType newRep = (PieceType)-1;

public:
	JokerTransform() { }
	JokerTransform(Position& _pos, PieceType _type) : pos(_pos), newRep(_type) {};

	bool operator==(const JokerTransform& other) const { return pos == other.getJokerChangePosition() && newRep == other.getRep(); }

	virtual const Point& getJokerChangePosition() const override { return pos; }
	virtual char getJokerNewRep() const override { return getPieceTypeRep(newRep); }
	PieceType getRep() const { return newRep; }
};


#endif // !JOKER_TRANSFORM

