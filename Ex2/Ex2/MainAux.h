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
#include "Game.h"

// maximum number of piece types to position on the board
#define ROCK_COUNT 2
#define PAPER_COUNT 5
#define SCISSORS_COUNT 1
#define BOMB_COUNT 2
#define JOKER_COUNT 2
#define FLAG_COUNT 1

// parse playerPositionsPath and place pieces in game.board
// return ParserMessageType and line error number
tuple<ParserMessageType, int> placePlayerPieces(Game* game, int playerNum, ifstream& infile);

// merge 2 boards into a new one
// perform a fight in every square with a collision
Game* gameVSgame(Game& game1, Game& game2);

// parse and apply all moves, write errors to outFile
// return true if game is over with a winner
bool applyAllMoves(Game& game, ifstream& movesFile1, ifstream& movesFile2, ofstream& outFile);

// treat writing positioning result (of parsing and placing) to outFile
// return true if there was an error
bool printPositioningResult(tuple<ParserMessageType, int> tuple1, tuple<ParserMessageType, int> tuple2, ofstream& outFile);

// treat writing GameVsGame result to outFile and console
// return true if game is over
bool printGameVsGameResult(GameStatus status, ofstream& outFile);

#endif