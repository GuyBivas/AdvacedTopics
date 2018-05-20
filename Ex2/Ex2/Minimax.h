#ifndef MINIMAX
#define MINIMAX

#include <stdlib.h>
#include <iostream>
#include <stdbool.h>
#include <vector>
#include <random>

#include "Position.h"
#include "GameBoard.h"
#include "AlgoPiece.h"
#include "Commands.h"

#define MINIMAX_DEPTH 2


#define WIN_SCORE 10000
#define INF_SCORE 100000

#define PAPER_SCORE 20
#define ROCK_SCORE 30
#define SCISSORS_SCORE 60
#define BOMB_SCORE 30
#define JOKER_SCORE 100
#define FLAG_SCORE 1000

std::pair<MoveCommand*, float> minimaxSuggestMove(GameBoard& game);

#endif