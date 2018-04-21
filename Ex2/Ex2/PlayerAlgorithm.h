#ifndef PLAYER_ALGORITHM
#define PLAYER_ALGORITHM

#include <vector>
#include <memory>
#include "Board.h"
#include "Move.h"
#include "FightInfo.h"
#include "JokerChange.h"

class PlayerAlgorithm {

public:
	virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) = 0;
	virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) = 0;
	virtual void notifyOnOpponentMove(const Move& move) = 0;
	virtual void notifyFightResult(const FightInfo& fightInfo) = 0;
	virtual unique_ptr<Move> getMove() = 0;
	virtual unique_ptr<JokerChange> getJokerChange() = 0;
};

#endif // !PLAYER_ALGORITHM