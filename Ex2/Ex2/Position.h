#ifndef POSITION
#define POSITION

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <algorithm>

#include "Point.h"
#include "Move.h"


/////////////////////////
//extern std::ofstream LOGFILE;
/////////////////////////

#define ROWS 10
#define COLS 10

// position on the board. the class does not know the board size
class Position : public Point
{
private:
	int x;
	int y;

public:
	Position() {};
	Position(int _x, int _y) : x(_x), y(_y) {};
	Position(const Point& pos) : x(pos.getX()), y(pos.getY()) {};

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
	Position operator-(const Point& other) const
	{
		return Position(x - other.getX(), y - other.getY());
	}

	Position operator+(const Point& other) const
	{
		return Position(x + other.getX(), y + other.getY());
	}

	int distance(const Point& other) const
	{
		return (Position(x - other.getX(), y - other.getY())).magnitude(); // TODO: optimize
	}

	int distanceFromCenter() const
	{
		return distance(Position(ROWS / 2, COLS / 2));
	}

	int distanceFromCorner() const
	{
		return (ROWS + COLS) / 2 + distanceFromCenter();
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
	bool isInBoard() const
	{
		return (x <= ROWS && y <= COLS && x > 0 && y > 0);
	}

	int isOnSide() const
	{
		return std::min({ x, ROWS - x, y, COLS - y }) == 0;
	}

	bool operator<(const Position& o2) const {
		int size1 = getX() * COLS + getY();
		int size2 = o2.getX() * COLS + o2.getY();
		return size1 < size2;
		//return ((getX() < o2.getX() && getY() < o2.getY()));// || (!(getX() < o2.getX()) && getY() < o2.getX()));
		//return ((getX() < o2.getX() && getY() < o2.getY()) || (!(getX() < o2.getX()) && getY() < o2.getX()));
	}

};

bool operator==(const Point& p1, const Point& p2);

class GameMove : public Move
{
private:
	Position from;
	Position to;

public:
	GameMove(Point& _from, Point& _to) : from(_from), to(_to) {};
	GameMove(Position _from, Position _to) : from(_from), to(_to) {};

	void operator=(const GameMove& other) {
		from = other.getFrom();
		to = other.getTo();
	}

	bool operator==(const GameMove& other) const { return from == other.getFrom() && to == other.getTo(); };

	virtual const Point& getFrom() const { return from; }
	virtual const Point& getTo() const { return to; }

	Position diff() const {
		return Position(to.getX() - from.getX(), to.getY() - from.getY());
	}
};

#endif