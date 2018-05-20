#include <string>
#include <fstream>
#include <map>
#include <iostream>

#include "MainAux.h"
#include "Piece.h"
#include "Parser.h"
#include "GameBoard.h"
#include "AutoPlayerAlgorithm.h"
#include "FilePlayerAlgorithm.h"

#define OUTPUT_PATH "rps.output"

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "Expected 1 argument (type of game). Recieved " << argc - 1 << "." << endl;
		return EXIT_FAILURE;
	}

	//LOGFILE = ofstream("LOGFILE4");

	PlayerAlgorithm* player1;
	PlayerAlgorithm* player2;

	string arg = argv[1];
	if (arg == "auto-vs-file") {
		player1 = new AutoPlayerAlgorithm();
		player2 = new FilePlayerAlgorithm();
	}
	else if (arg == "file-vs-auto") {
		player1 = new FilePlayerAlgorithm();
		player2 = new AutoPlayerAlgorithm();
	}
	else if (arg == "auto-vs-auto") {
		player1 = new AutoPlayerAlgorithm();
		player2 = new AutoPlayerAlgorithm();
	}
	else if (arg == "file-vs-file") {
		player1 = new FilePlayerAlgorithm();
		player2 = new FilePlayerAlgorithm();
	}
	else {
		cout << "Invalid command line argument." << endl;
		return EXIT_FAILURE;
	}

	//player1 = new AutoPlayerAlgorithm();
	//player2 = new AutoPlayerAlgorithm();

	//player1 = new FilePlayerAlgorithm();
	//player2 = new FilePlayerAlgorithm();

	ofstream outFile(OUTPUT_PATH);

	GameManager gameManager(*player1, *player2, outFile);

	gameManager.runGame();

	outFile.close();

	delete(player1);
	delete(player2);

	return EXIT_SUCCESS;
}