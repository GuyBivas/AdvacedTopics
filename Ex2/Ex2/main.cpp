#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include "MainAux.h"
#include "Piece.h"
#include "Parser.h"
#include "Game.h"

#define positionsPathPlayer1 "player1.rps_board"
#define positionsPathPlayer2 "player2.rps_board"
#define movesPathPlayer1 "player1.rps_moves"
#define movesPathPlayer2 "player2.rps_moves"
#define outputPath "rps.output"

int main()
{
	ifstream positionFilePlayer1(positionsPathPlayer1);
	ifstream positionFilePlayer2(positionsPathPlayer2);
	ifstream movesFilePlayer1(movesPathPlayer1);
	ifstream movesFilePlayer2(movesPathPlayer2);

	if (!positionFilePlayer1.good() || !positionFilePlayer2.good() ||
		!movesFilePlayer1.good() || !movesFilePlayer2.good())
	{
		cout << "Error opening files." << endl;
		return EXIT_FAILURE;
	}

	ofstream outFile(outputPath);

	Game* player1Board = new Game();
	Game* player2Board = new Game();

	tuple<ParserMessageType, int> tuple1 = placePlayerPieces(player1Board, 1, positionFilePlayer1);
	tuple<ParserMessageType, int> tuple2 = placePlayerPieces(player2Board, 2, positionFilePlayer2);

	positionFilePlayer1.close();
	positionFilePlayer2.close();

	bool gameEnded = printPositioningResult(tuple1, tuple2, outFile);

	if (gameEnded)
	{
		delete(player1Board);
		delete(player2Board);
	}
	else
	{
		Game* game = gameVSgame(*player1Board, *player2Board);
		delete(player1Board);
		delete(player2Board);

		GameStatus status = game->getGameStatus();
		gameEnded = printGameVsGameResult(status, outFile);

		if (!gameEnded)
		{
			gameEnded = applyAllMoves(*game, movesFilePlayer1, movesFilePlayer2, outFile);

			if (!gameEnded)
			{
				outFile << "Winner: 0" << endl;
				outFile << "Reason: A tie - both Moves input files done without a winner" << endl;
			}
		}
		
		movesFilePlayer1.close();
		movesFilePlayer2.close();

		outFile << endl << game->getBoardRep(); // spacing line
		outFile.close();
		delete(game);
		return EXIT_SUCCESS;
	}
}