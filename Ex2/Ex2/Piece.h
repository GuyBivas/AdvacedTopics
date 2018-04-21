#ifndef Piece_H
#define Piece_H

#include <cstdlib>
#include "Position.h"

enum PieceType
{
	Rock,
	Paper,
	Scissors,
	Bomb,
	Flag,
	Joker
};

enum FightResult
{
	FightWin,
	FightDraw,
	FightLose
};

// represent a piece in the board
class Piece
{
private:
	PieceType type;
	Position pos;
	int playerNum;
	bool isJoker;

public:
	Piece(PieceType _type, Position _pos, int _playerNum, bool _isJoker = false) :
		type(_type), pos(_pos), playerNum(_playerNum), isJoker(_isJoker) {};

	Piece(Piece& piece) : type(piece.type), pos(piece.pos), playerNum(piece.playerNum), isJoker(piece.isJoker) {};

	void operator=(Piece other)
	{
		type = other.type;
		pos = other.pos;
		playerNum = other.playerNum;
		isJoker = other.isJoker;
	}

	// get the result of a fight between this and other piece (win\lose\draw)
	FightResult getFightResult(const Piece* other) const;

	// get char representation of this piece
	char getRep() const;

	// setters
	void setPos(Position _pos) { pos = _pos; };
	void setType(PieceType _type) { type = _type; };

	// getters
	PieceType getType() const { return type; }
	Position getPos() const { return pos; }
	int getPlayerNum() const { return playerNum; }
	bool getIsJoker() const { return isJoker; }
};

// copy piece or return null (if given null)
Piece* copyPiece(Piece* piece);

#endif