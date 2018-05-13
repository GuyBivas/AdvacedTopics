#ifndef GAME_MANAGER
#define GAME_MANAGER

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>

#include "FilePlayerAlgorithm.h"
#include "Fight.h"
#include "Parser.h"
#include "GameBoard.h"
#include "JokerTransform.h"

#define MAX_TURNS_SINCE_FIGHT 100

class GameManager {

private:
	GameBoard board;
	PlayerAlgorithm& player1;
	PlayerAlgorithm& player2;
	ofstream& outFile;
	int turnsSinceFight = 0;

	void gameVSgame(GameBoard& game1, GameBoard& game2, vector<unique_ptr<FightInfo>>& fights);
	bool placePlayerPieces(GameBoard& board, PlayerAlgorithm& player, int playerNum);

public:

	GameManager (PlayerAlgorithm& _player1, PlayerAlgorithm& _player2, ofstream& _outputFile) :
		player1(_player1), player2(_player2), outFile(_outputFile) {
		board = GameBoard();
	}

	bool initBoard(); // initializes the board by calling the init board of both players
	
	std::pair<bool, bool> applyMove(int playerNum); // apply one move of the current player

	bool getJokerChange(int playerNum);

	void runGame(); // controls the flow of the game
};

#endif