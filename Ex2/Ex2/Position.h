#ifndef POSITION
#define POSITION

#include <cstdlib>
#include <cmath>
#include "Point.h"
#include "Move.h"

#define ROWS 10
#define COLS 10

// position on the board. the class does not know the board size
class Position : public Point {
private:
	int x;
	int y;

public:
	Position(int _x, int _y) : x(_x), y(_y) {};
	Position(const Position& pos) : x(pos.x), y(pos.y) {};

	// getters
	int getX() const override { return x; }
	int getY() const override { return y; }

	bool operator==(Point& other)
	{
		return x == other.getX() && y == other.getY();
	}

	void operator=(Point& other)
	{
		x = other.getX();
		y = other.getY();
	}

	// difference - use Position as Vector2
	Position operator-(Point const& other) const
	{
		return Position(x - other.getX(), y - other.getY());
	}

	// absolute value on each coordinate - use Position as Vector2
	Position abs()
	{
		return Position(std::abs(x), std::abs(y));
	}

	// abs sum of coordinates - use Position as Vector2
	int magnitude()
	{
		Position p = abs();
		return p.x + p.y;
	}

	// check that x and y are within the board
	bool isInBoard(int rows = ROWS, int cols = COLS) const
	{
		return (x <= cols && y <= rows && x > 0 && y > 0);
	}
};


class GameMove : public Move {

private:
	Point & from;
	Point& to;

public:
	GameMove(Point& _from, Point& _to) : from(_from), to(_to) {};

	void operator=(const GameMove& other) {
		from = other.getFrom();
		to = other.getTo();
	}

	virtual const Point& getFrom() const { return from; }
	virtual const Point& getTo() const { return to; }

	Position diff() const {
		return Position(to.getX() - from.getX(), to.getY() - from.getY());
	}
};

#endif