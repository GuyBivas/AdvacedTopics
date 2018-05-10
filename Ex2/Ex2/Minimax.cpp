#include "Minimax.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))


const std::vector<Position> directions = { Position(-1,0), Position(1,0), Position(0,1), Position(0,-1) };
const std::vector<PieceType> jokerReps = { Scissors, Paper, Bomb, Rock };

vector<Position> getPlayersJokersPos(GameBoard& game) {
	vector<Position> positions = {};

	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Position pos = Position(j, i);
			Piece* piece = game[pos];

			if ((piece != nullptr) && (piece->getPlayerNum() == game.getCurrentPlayer()) && (piece->getIsJoker())) {
				positions.push_back(pos);
			}
		}
	}
	
	return positions;
}

int getPieceScore(Piece* piece)
{
	int score;

	if (piece == nullptr)
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

int calcBoardScore(GameBoard& game) {
	int score = 0;

	for (int i = 1; i <= ROWS; i++)
	{
		for (int j = 1; j <= COLS; j++)
		{
			Position pos = Position(j, i);
			score += getPieceScore(game[pos]);
		}
	}

	return score;
}

int getGameScore(GameBoard& game)
{
	int score = 0;

	GameStatus status = game.getGameStatus();
	switch (status)
	{
	case StatusNormal:
		score = calcBoardScore(game);
		break;
	case StatusPlayer1FlagsCaptured:
	case StatusPlayer1NoMovingPieces:
		score = -WIN_SCORE;
		break;
	case StatusPlayer2FlagsCaptured:
	case StatusPlayer2NoMovingPieces:
		score = WIN_SCORE;
		break;
	case StatusBothPlayersNoMovingPieces:
		score = 0; // TODO: maybe wins?
		break;
	default:
		break;
	}

	return score;
}

int minimax(GameBoard& game, int depth, int a, int b, int color, MoveCommand* command)
{
	if (depth == 0 || game.isGameOver()) {
		return getGameScore(game) * color;
	}
	
	int moveScore;
	int bestValue = -INF_SCORE;

	for (int y = 1; y <= ROWS; y++) {
		for (int x = 1; x <= COLS; x++) {
			Position pos = Position(x, y);
			Piece* piece = game[pos];

			if ((piece != nullptr) && (piece->getPlayerNum() == game.getCurrentPlayer()) && (piece->getIsMovingPiece())) {
				for (Position direction : directions) { // go over all possible moves
					Position to = pos + direction;
					if (to.isInBoard() && (game[to] == nullptr || game[to]->getPlayerNum() != game.getCurrentPlayer())) {
						GameBoard gameCopy = GameBoard(game);
						GameMove move = GameMove(pos, to);
						gameCopy.move(move);
						gameCopy.setCurrPlayer(gameCopy.getOtherPlayer());
						
						// change joker
						vector<Position> jokerPositions = getPlayersJokersPos(game); 
						for (Position jokerPos : jokerPositions) {
							PieceType jokerOriginType = getPieceType(game[jokerPos]->getJokerRep());
							
							for (PieceType newType : jokerReps) {
								gameCopy.changeJoker(jokerPos, newType);
								moveScore = -minimax(gameCopy, depth - 1, -b, -a, -color, command);
								if (moveScore > bestValue) {
									bestValue = moveScore;
									if (depth == MINIMAX_DEPTH) { // update MoveCommand
										JokerTransform jokerTrans = JokerTransform(jokerPos, newType);
										delete(command);
										command = new MoveCommand(move, jokerTrans);
									}
								}

								// pruning
								a = max(a, moveScore);
								if (a >= b) {
									return bestValue;
								}

								gameCopy.changeJoker(jokerPos, jokerOriginType); // undo joker change
							}
						}
					}
				}
			}
		}
	}
	
	return bestValue;
}

std::pair<MoveCommand*, int> minimaxSuggestMove(GameBoard& game) {
	MoveCommand* command = nullptr;
	int score = minimax(game, MINIMAX_DEPTH, -INF_SCORE, INF_SCORE, (game.getCurrentPlayer() == 1 ? 1 : -1), command);

	return std::pair<MoveCommand*, int>(command, score);
}