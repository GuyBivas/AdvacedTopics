#ifndef COMMANDS
#define COMMANDS

#include "Piece.h"
#include "Position.h"
#include "Parser.h"
#include "JokerTransform.h"

// all outcomes of parsing postioning command or move command
enum ParserMessageType
{
	ParseOK,
	ParseInvalidCommand,
	ParseOutOfBounds,
	ParseMultiplePiecesInPos,
	ParseInvalidJokerRep,
	ParsePositionedTooManyPieces,
	ParseNotPositionedAllFlags
};


// class for parsing a postioning command, holds all necessary data
class PositioningCommand
{
private:
	ParserMessageType messageType;
	PieceType type;
	Position pos = Position(-1,-1);
	bool isJoker;

public:
	PositioningCommand(ParserMessageType _msgType, PieceType _pieceType, Position _pos, bool _isJoker) :
		messageType(_msgType), type(_pieceType), pos(_pos), isJoker(_isJoker) {};

	PositioningCommand(ParserMessageType _msgType) :
		messageType(_msgType) {};

	// getters
	PieceType getType() const { return type; }
	Point& getPos() { return pos; }
	bool getIsJoker() const { return isJoker; }
	ParserMessageType getMessageType() const { return messageType; }
};


class MoveCommand
{
private:
	ParserMessageType messageType;
	GameMove move;
	JokerTransform jokerChange;
	//Position jokerPos;
	//PieceType jokerNewRep = (PieceType)-1;

public:
	MoveCommand(ParserMessageType _msgType, GameMove& _move, JokerTransform& _jokerChange) :
		messageType(_msgType), move(_move), jokerChange(_jokerChange) {};

	// getters
	ParserMessageType getMessageType() const { return messageType; }
	const GameMove& getMove() const { return move; }
	const Point& getJokerPos() const { return jokerChange.getJokerChangePosition(); }
	PieceType getJokerNewRep() const { return jokerChange.getRep(); }
	JokerTransform getJokerTransform() const { return jokerChange; }
};

#endif // !COMMANDS