#ifndef MAINAUX_H
#define MAINAUX_H

#include <string>
#include <cstdio>
#include <fstream>
#include <map>
#include <iostream>
#include "MainAux.h"
#include "Piece.h"
#include "Parser.h"
#include "GameBoard.h"
#include "GameManager.h"

// parse playerPositionsPath and place pieces in game.board
// return ParserMessageType and line error number
tuple<ParserMessageType, int> placePlayerPieces(GameBoard* game, int playerNum, ifstream& infile);

// merge 2 boards into a new one
// perform a fight in every square with a collision
GameBoard* gameVSgame(GameBoard& game1, GameBoard& game2);

// parse and apply all moves, write errors to outFile
// return true if game is over with a winner
bool applyAllMoves(GameBoard& game, ifstream& movesFile1, ifstream& movesFile2, ofstream& outFile);

// treat writing positioning result (of parsing and placing) to outFile
// return true if there was an error
bool printPositioningResult(tuple<ParserMessageType, int> tuple1, tuple<ParserMessageType, int> tuple2, ofstream& outFile);

// treat writing GameVsGame result to outFile and console
// return true if game is over
bool printGameVsGameResult(GameStatus status, ofstream& outFile);

#endif