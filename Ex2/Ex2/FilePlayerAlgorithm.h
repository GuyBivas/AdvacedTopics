#ifndef FILE_PLAYERALGORITHM
#define FILE_PLAYERALGORITHM

#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <functional>  
#include <memory>

#include "Parser.h"
#include "PlayerAlgorithm.h"
#include "PlayerAlgorithm.h"
#include "GameBoard.h"
#include "Fight.h"
#include "Commands.h"

#define positionsPathPlayer1 "player1.rps_board"
#define positionsPathPlayer2 "player2.rps_board"
#define movesPathPlayer1 "player1.rps_moves"
#define movesPathPlayer2 "player2.rps_moves"

using namespace std;

class FilePlayerAlgorithm : public PlayerAlgorithm 
{
private:
	vector<ParserMoveCommand> moveCommands;
	size_t index = 0;

public:
	virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill);
	virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) { };
	virtual void notifyOnOpponentMove(const Move& move) { }; // called only on opponent’s move
	virtual void notifyFightResult(const FightInfo& fightInfo) { }; // called only if there was a fight
	
	virtual unique_ptr<Move> getMove() { 
		if (index >= moveCommands.size())
			return nullptr;

		return make_unique<GameMove>(moveCommands[index].getMove()); 
	};

	virtual unique_ptr<JokerChange> getJokerChange();

	virtual ~FilePlayerAlgorithm() { }

};

#endif