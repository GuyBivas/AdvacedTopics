#ifndef AUTO_PLAYER_ALGORITHM
#define AUTO_PLAYER_ALGORITHM

#include <map>
#include <math.h>
#include <algorithm>
#include <random>
#include "PlayerAlgorithm.h"
#include "GameBoard.h"
#include "AlgoPiece.h"
#include "Fight.h"
#include "Minimax.h"

using namespace std;

class AutoPlayerAlgorithm : public PlayerAlgorithm {
private:
	GameBoard playerBoard;
	GameBoard opponentBoard;
	int playerNum;
	map<PieceType, int> enemyPieceCount = {
		{ Rock, ROCK_COUNT + JOKER_COUNT },
		{ Paper, PAPER_COUNT + JOKER_COUNT },
		{ Scissors, SCISSORS_COUNT + JOKER_COUNT },
		{ Bomb, BOMB_COUNT },
		{ Flag, FLAG_COUNT } };
	JokerChange* lastMoveJokerChange = NULL;


	void guessOpponentPieces(GameBoard& toFill);
	PieceType choosePieceTypeProbabilty();

public:
	virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) override;
	virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) override;
	virtual void notifyOnOpponentMove(const Move& move) override; // called only on opponent’s move
	virtual void notifyFightResult(const FightInfo& fightInfo) override; // called only if there was a fight
	virtual unique_ptr<Move> getMove() = 0;
	virtual unique_ptr<JokerChange> getJokerChange() = 0; // nullptr if no change is requested
	virtual ~AutoPlayerAlgorithm() {}
};


#endif