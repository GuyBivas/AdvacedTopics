#ifndef PARSER
#define PARSER

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "Piece.h"
#include "Commands.h"

static class Parser {

public:
	static PositioningCommand parsePositioningCommand(std::string line);
	static MoveCommand parseMoveCommand(std::string line);
};

#endif