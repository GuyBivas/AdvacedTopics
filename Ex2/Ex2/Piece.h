#ifndef PIECE
#define PIECE

#include <cstdlib>
#include "Position.h"
#include "PiecePosition.h"

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
class Piece : public PiecePosition
{
private:
	PieceType type;
	Position pos;
	int playerNum;
	bool isJoker;

public:
	Piece(PieceType _type, const Point& _pos, int _playerNum, bool _isJoker = false) :
		type(_type), pos(_pos), playerNum(_playerNum), isJoker(_isJoker) {};

	Piece(const Piece& piece) : type(piece.type), pos(piece.pos), playerNum(piece.playerNum), isJoker(piece.isJoker) {};

	virtual void operator=(Piece& other)
	{
		type = other.type;
		pos = other.pos;
		playerNum = other.playerNum;
		isJoker = other.isJoker;
	}

	// get the result of a fight between this and other piece (win\lose\draw)
	virtual FightResult getFightResult(const Piece* other) const;

	// setters
	virtual void setPos(const Point& _pos) { pos = _pos; };
	virtual void setType(PieceType _type) { type = _type; };

	// getters
	virtual PieceType getType() const { return type; }
	virtual int getPlayerNum() const { return playerNum; }
	virtual bool getIsJoker() const { return isJoker; }
	virtual bool getIsMovingPiece() const { return type == Rock || type == Paper || type == Scissors; }
	virtual char getPiece() const; // get char representation of this piece
	virtual char getJokerRep() const;
	virtual const Point& getPosition() const override { return pos; }
	char getRep() const { return (getIsJoker()) ? getJokerRep() : getPiece(); }
};

// copy piece or return null (if given null)
Piece* copyPiece(Piece* piece);
char getPieceTypeRep(PieceType type);
PieceType getPieceType(char c);

#endif