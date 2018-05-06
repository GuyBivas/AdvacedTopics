#ifndef PARSER
#define PARSER

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "Piece.h"
#include "Commands.h"

using namespace std;

PositioningCommand parsePositioningCommand(string line);
ParserMoveCommand parseMoveCommand(string line);

#endif