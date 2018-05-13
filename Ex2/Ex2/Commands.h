#ifndef COMMANDS
#define COMMANDS

#include "Piece.h"
#include "Position.h"
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
	Position& getPos() { return pos; }
	bool getIsJoker() const { return isJoker; }
	ParserMessageType getMessageType() const { return messageType; }
};


class MoveCommand
{
private:
	GameMove move;
	JokerTransform jokerChange;

public:
	MoveCommand(GameMove _move, JokerTransform _jokerChange) :
		move(_move), jokerChange(_jokerChange) {};

	bool operator==(const MoveCommand& other) const { return (move == other.getMove()) && (jokerChange == other.getJokerTransform()); };

	// setters
	void setMove(GameMove _move) { move = _move; }
	void setJokerTransform(JokerTransform _jokerChange) { jokerChange = _jokerChange; }

	// getters
	const GameMove& getMove() const { return move; }
	const Point& getJokerPos() const { return jokerChange.getJokerChangePosition(); }
	PieceType getJokerNewRep() const { return jokerChange.getRep(); }
	JokerTransform getJokerTransform() const { return jokerChange; }
};

class ParserMoveCommand : public MoveCommand {
private:
	ParserMessageType messageType;

public:
	ParserMoveCommand(ParserMessageType _msgType, GameMove& _move, JokerTransform& _jokerChange) :
		MoveCommand(_move,_jokerChange),messageType(_msgType) {};

	ParserMessageType getMessageType() const { return messageType; }
};

namespace std
{
	template <>
	struct hash<MoveCommand>
	{
		size_t operator()(const MoveCommand& move) const
		{
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return ((hash<int>()(move.getMove().getFrom().getX()) ^ (hash<int>()(move.getMove().getTo().getY()) << 1)) >> 1);
		}
	};
}

#endif // !COMMANDS