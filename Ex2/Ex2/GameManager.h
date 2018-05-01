#ifndef GAME_MANAGER
#define GAME_MANAGER

#include "GameBoard.h"
#include "FilePlayerAlgorithm.h"
#include <stdlib.h>
#include "Parser.h"
#include "Fight.h"
#include <iostream>
#include <fstream>

class GameManager {

private:
	GameBoard board;
	PlayerAlgorithm& player1;
	PlayerAlgorithm& player2;
	ofstream& outFile;

	void gameVSgame(GameBoard& game1, GameBoard& game2, vector<unique_ptr<FightInfo>>& fights1, vector<unique_ptr<FightInfo>>& fights2);
	bool movePlayerPiece(GameBoard& game, MoveCommand moveCommand, int lineNumber);

public:

	GameManager (PlayerAlgorithm& _player1, PlayerAlgorithm& _player2, ofstream& _outputFile) :
		player1(_player1), player2(_player2), outFile(_outputFile) {
		board = GameBoard();
	}

	void initBoard(); // initializes the board by calling the init board of both players
	
	void applyMove(PlayerAlgorithm& player, int playerNum); // apply one move of the current player

	void runGame(); // controls the flow of the game



};


#endif // !GAME_MANAGER
