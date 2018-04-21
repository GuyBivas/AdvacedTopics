#ifndef Position_H
#define Position_H

#include <cstdlib>
#include <cmath>

#define ROWS 10
#define COLS 10

// position on the board. the class does not know the board size
class Position
{
private:
	int x;
	int y;

public:
	Position(int _x, int _y) : x(_x), y(_y) {};

	bool operator==(Position other)
	{
		return x == other.x && y == other.y;
	}

	void operator=(Position other)
	{
		x = other.x;
		y = other.y;
	}

	// difference - use Position as Vector2
	Position operator-(Position const other) const
	{
		return Position(x - other.x, y - other.y);
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

	// getters
	int getRow() const { return y; }
	int getCol() const { return x; }
};

#endif