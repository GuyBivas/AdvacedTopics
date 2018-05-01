#include "Minimax.h"
//
//#define max(a,b) (((a) > (b)) ? (a) : (b))
///*
//int minimax(GameBoard& game, int depth, int a, int b, int color, MoveCommand* command)
//{
//	//GameStatus status = game.getGameStatus();
//
//	//if (depth == 0 || game->status == STATUS_DRAW || game->status == STATUS_MATE)
//	{
//		return getGameScore(game) * color;
//	}
//	//else
//	{
//		int moveScore, bestMove;
//		int bestValue = -INF_SCORE;
//		//MoveOptionsList* moves = gameGetAllValidMoves(game, game->currentPlayer, false);
//
//		vector<Position> jokerPositions = getPlayersJokersPos(game);
//
//		for (int y = 1; y <= ROWS; y++) {
//			for (int x = 1; x <= COLS; x++) {
//
//				Position pos = Position(x, y);
//				Piece* piece = game[pos];
//
//				if ((piece != NULL) && (piece->getPlayerNum() == game.getCurrentPlayer()) && (piece->getIsMovingPiece())) {
//					
//						// for each direction - apply all joker type moves
//
//				}
//			}
//		}
//
//
//		//for (int i = 0; i < moves->actualSize; i++)
//		//{
//		//	ChessGame* copy = gameCopy(game);
//		//	gameMakeMove(copy, arrayListGetAt(moves, i)->move);
//
//		//	moveScore = -minimax(copy, depth - 1, -b, -a, -color, false);
//		//	gameDestroy(copy);
//
//		//	if (moveScore > bestValue)
//		//	{
//		//		bestValue = moveScore;
//		//		bestMove = i;
//		//	}
//
//		//	// pruning
//		//	a = max(a, moveScore);
//		//	if (a >= b)
//		//		break;
//		//}
//
//		//arrayListDestroy(moves);
//
//		//return (returnMove ? bestMove : bestValue);
//	}
//}
//*/
//
///*
//MoveCommand minimaxSuggestMove(GameBoard& game)
//{
//	Position pos = Position(-1, -1);
//	GameMove move = GameMove(pos, pos);
//	JokerTransform jokerChange = JokerTransform(pos, Rock);
//	MoveCommand command = MoveCommand(ParseOK, move, jokerChange);
//	int moveIndex = minimax(game, DEPTH, -INF_SCORE, INF_SCORE, (game.getCurrentPlayer() == 1 ? 1 : -1), &command);
//
//	return command;
//}
//*/
//
int getPieceScore(Piece* piece)
{
	int score;

	if (piece == NULL)
		return 0;
	
	switch (piece->getType())
	{
	case Rock:
		score = ROCK_SCORE;
		break;
	case Paper:
		score = PAPER_SCORE;
		break;
	case Scissors:
		score = SCISSORS_SCORE;
		break;
	case Bomb:
		score = BOMB_SCORE;
		break;
	case Flag:
		score = FLAG_SCORE;
		break;
	case Joker:
		score = JOKER_SCORE;
		break;
	default:
		break;
	}

	return (piece->getPlayerNum() == 1 ? score : -score);
}

//int calcBoardScore(GameBoard& game) {
//	int score = 0;
//	
//	for (int i = 1; i <= ROWS; i++)
//	{
//		for (int j = 1; j <= COLS; j++)
//		{
//			Position pos = Position(j, i);
//			score += getPieceScore(game[pos]);
//		}
//	}
//
//	return score;
//}
//
//int getGameScore(GameBoard& game)
//{
//	int score = 0;
//
//	GameStatus status = game.getGameStatus();
//	switch (status)
//	{
//	case StatusNormal:
//		score = calcBoardScore(game);
//		break;
//	case StatusPlayer1FlagsCaptured:
//	case StatusPlayer1NoMovingPieces:
//		score = -WIN_SCORE;
//		break;
//	case StatusPlayer2FlagsCaptured:
//	case StatusPlayer2NoMovingPieces:
//		score = WIN_SCORE;
//		break;
//	case StatusBothPlayersNoMovingPieces:
//		score = 0; // TODO: maybe wins?
//		break;
//	default:
//		break;
//	}
//
//	return score;
//}
//
//vector<Position> getPlayersJokersPos(GameBoard& game) {
//	vector<Position> positions = {};
//
//	for (int i = 1; i <= ROWS; i++)
//	{
//		for (int j = 1; j <= COLS; j++)
//		{
//			Position pos = Position(j, i);
//			Piece* piece = game[pos];
//
//			if ((piece != NULL) && (piece->getPlayerNum() == game.getCurrentPlayer()) && (piece->getIsJoker())) {
//
//			}
//		}
//	}
//}
//
//vector<MoveCommand> getAllMoves(GameBoard& game, int playerNum) {
//	vector<MoveCommand> moves = {};
//	
//	
//
//	for (int i = 1; i <= ROWS; i++)
//	{
//		for (int j = 1; j <= COLS; j++)
//		{
//			Position pos = Position(j, i);
//			Piece* piece = game[pos];
//
//			if (piece != NULL && piece->getPlayerNum() == playerNum && piece->getIsMovingPiece()) {
//				
//			}
//		}
//	}
//}