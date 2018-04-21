#ifndef Parser_H
#define Parser_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "Piece.h"

using namespace std;

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
class PostioningCommand
{
private:
	ParserMessageType messageType;
	PieceType type;
	Position pos = Position(-1, -1); // put garbage to compile
	bool isJoker;

public:
	// constructor get a line and extract all necessary data
	PostioningCommand(string line);

	// getters
	PieceType getType() const { return type; }
	Position getPos() const { return pos; }
	bool getIsJoker() const { return isJoker; }
	ParserMessageType getMessageType() const { return messageType; }
};

// class for parsing a move command and hold all necessary data
class MoveCommand
{
private:
	ParserMessageType messageType;
	Position from = Position(-1, -1); // put garbage to compile
	Position to = Position(-1, -1); // put garbage to compile
	Position jokerPos = Position(-1, -1); // put garbage to compile
	PieceType jokerNewRep = (PieceType)-1;

public:
	// constructor get a line and extract all necessary data
	MoveCommand(string line);

	// getters
	ParserMessageType getMessageType() const { return messageType; }
	Position getFrom() const { return from; }
	Position getTo() const { return to; }
	Position getJokerPos() const { return jokerPos; }
	PieceType getJokerNewRep() const { return jokerNewRep; }
};

#endif