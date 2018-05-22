#ifndef AUTO_PLAYER_ALGORITHM
#define AUTO_PLAYER_ALGORITHM

#include <math.h>
#include <map>
#include <random>
#include <unordered_map>
#include <functional>  
#include <algorithm>

#include "PlayerAlgorithm.h"
#include "GameBoard.h"
#include "AlgoPiece.h"
#include "Fight.h"
#include "Minimax.h"

#define GUESS_AMOUNT 1
#define MINIMAX_DEPTH 3

using namespace std;

class AutoPlayerAlgorithm : public PlayerAlgorithm {
private:
	GameBoard playerBoard;
	GameBoard opponentBoard;
	int playerNum;
	map<PieceType, int> enemyPieceCount = {
		{ Rock, ROCK_COUNT },
		{ Paper, PAPER_COUNT },
		{ Scissors, SCISSORS_COUNT },
		{ Bomb, BOMB_COUNT },
		{ Flag, FLAG_COUNT } };
	JokerTransform lastMoveJokerChange;


	void guessOpponentPiecesByType(GameBoard& toFill, PieceType type, function<bool(AlgoPiece*)> condition, function<int(AlgoPiece*)> probabilty) const;
	void guessOpponentPieces(GameBoard& toFill, bool onlyJokers = false) const;
	PieceType choosePieceTypeProbabilty() const;
	int calcMinimaxDepth();

public:
	AutoPlayerAlgorithm() {}
	virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) override;
	virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) override;
	virtual void notifyOnOpponentMove(const Move& move) override; // called only on opponent’s move
	virtual void notifyFightResult(const FightInfo& fightInfo) override; // called only if there was a fight
	virtual unique_ptr<Move> getMove();
	virtual unique_ptr<JokerChange> getJokerChange(); // nullptr if no change is requested
	virtual ~AutoPlayerAlgorithm() { }
};

AlgoPiece* copyAlgoPiece(AlgoPiece* piece);


#endif