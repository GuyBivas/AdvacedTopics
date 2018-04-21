#ifndef MINIMAX
#define MINIMAX

#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include "Position.h"
#include "GameBoard.h"
#include "Commands.h"

#define DEPTH 6

#define WIN_SCORE 1000
#define INF_SCORE 10000

#define PAPER_SCORE 2
#define ROCK_SCORE 3
#define SCISSORS_SCORE 6
#define BOMB_SCORE 3
#define JOKER_SCORE 10
#define FLAG_SCORE 100

GameMove minimaxSuggestMove(GameBoard& game);

#endif