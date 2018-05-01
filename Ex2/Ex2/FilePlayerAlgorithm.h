#ifndef FILE_PLAYERALGORITHM
#define FILEPLAYERALGORITHM

#include "PlayerAlgorithm.h"

class FilePlayerAlgorithm : public PlayerAlgorithm {

private:

public:
	virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) = 0;
	virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) = 0;
	virtual void notifyOnOpponentMove(const Move& move) = 0; // called only on opponent�s move
	virtual void notifyFightResult(const FightInfo& fightInfo) = 0; // called only if there was a fight
	virtual unique_ptr<Move> getMove() = 0;
	virtual unique_ptr<JokerChange> getJokerChange() = 0; // nullptr if no change is requested
	//virtual ~FilePlayerAlgorithm() {}

};



#endif // !FILE_PLAYER_ALGORITHM

