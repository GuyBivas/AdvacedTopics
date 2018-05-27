#include "Minimax.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

int playerNumber;
int minimaxDepth;

const std::vector<Position> directions = { Position(-1,0), Position(1,0), Position(0,1), Position(0,-1) };
const std::vector<PieceType> jokerReps = { Rock, Paper, Scissors };
//const std::vector<PieceType> jokerReps = { Scissors, Paper, Rock };

int getPieceScore(Piece* piece)
{
	int score;
	PieceType type = (piece->getIsJoker() ? Joker : piece->getType());

	switch (type)
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

	// each piece on the board reduces the score by 1, causes the alogrithm to be agressive
	return (piece->getPlayerNum() == 1 ? score - playerNumber : -score - playerNumber);
}

// TODO: maybe add some weight to ratio
// add total distance from unmoved enemy pieces (get closer to flag)
// decrease score of pieces the enemy knows (-2)
// decrease score of pieces moved (-1)
float calcBoardScore(GameBoard* game)
{
	float totalX1 = 0;
	float totalY1 = 0;
	float totalX2 = 0;
	float totalY2 = 0;
	int pieceCount1 = 0;
	int pieceCount2 = 0;

	int score = 0;
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			AlgoPiece* piece = (AlgoPiece*)game->getPieceAt(j, i);
			if (piece == nullptr)
				continue;

			score += getPieceScore(piece);

			Position pos = piece->getPosition();
			if (piece->getPlayerNum() == ((playerNumber == 1) ? 1 : 2)) {
				if (piece->getIsMovingPiece()) {
					pieceCount1++;
					totalX1 += pos.getX();
					totalY1 += pos.getY();
				}
			}
			else if (!piece->getIsMovingPiece()) {
				pieceCount2++;
				totalX2 += pos.getX();
				totalY2 += pos.getY();
			}

			if (piece->getType() == Flag) {
				// you prefer to be closer to your flag and more to the enemy flag
				int flagLove = (piece->getPlayerNum() == (playerNumber == 1 ? 1 : 2)) ? 0 : 50; // TODO: 0->1
				pieceCount2 += flagLove;
				totalX2 += flagLove * pos.getX();
				totalY2 += flagLove * pos.getY();

				if (playerNumber == 0)
					for (Position direction : directions) {
						Position to = pos + direction;
						if (to.isInBoard() && game->getPieceAt(to) != nullptr && game->getPieceAt(to)->getPlayerNum() == (playerNumber == 1 ? 1 : 2)) {
							score += (to.isOnSide() ? to.distanceFromCorner() : 10) * playerNumber;
						}
					}
			}
		}
	}

	totalX1 /= pieceCount1;
	totalY1 /= pieceCount1;
	totalX2 /= pieceCount2;
	totalY2 /= pieceCount2;

	float distanceX = abs(totalX1 - totalX2);
	float distanceY = abs(totalY1 - totalY2);
	float avgDistance = (distanceX * distanceX + distanceY * distanceY) / 5 * 3;
	//cout << avgDistance << endl;
	
	if (pieceCount2 == 1)
		avgDistance *= 10;

	return score - avgDistance * playerNumber;
}

float getGameScore(GameBoard* game)
{
	float score = 0;
	GameStatus status = game->getGameStatus();
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

float minimax(GameBoard* game, int depth, float a, float b, int color, MoveCommand* command, bool skipMinNodes, bool tryJokerPermutations)
{
	if (depth == 0 || (game->isGameOver() && depth != minimaxDepth)) {
		return getGameScore(game) * color;
	}

	if (skipMinNodes && depth % 2 == 0)
		return -minimax(game, depth - 1, -b, -a, -color, command, skipMinNodes, tryJokerPermutations);

	float moveScore;
	float bestValue = -INF_SCORE;

	for (int y = 1; y <= ROWS; y++) {
		for (int x = 1; x <= COLS; x++) {
			Position pos = Position(x, y);
			Piece* piece = game->getPieceAt(pos);

			if ((piece != nullptr) && (piece->getPlayerNum() == game->getCurrentPlayer()) && (piece->getIsMovingPiece())) {
				for (Position direction : directions) { // go over all possible moves
					Position to = pos + direction;
					if (to.isInBoard() && (game->getPieceAt(to) == nullptr || game->getPieceAt(to)->getPlayerNum() != game->getCurrentPlayer())) {
						GameBoard* gameCopy = new GameBoard(*game);
						GameMove move = GameMove(pos, to);
						gameCopy->move(move);
						gameCopy->setCurrPlayer(gameCopy->getOtherPlayer());

						// change joker
						vector<Position> jokerPositions = {};
						if (tryJokerPermutations && depth == minimaxDepth)
							jokerPositions = game->getPlayersJokersPos();

						if (jokerPositions.empty()) {
							moveScore = -minimax(gameCopy, depth - 1, -b, -a, -color, command, skipMinNodes, tryJokerPermutations);
							if (moveScore >= bestValue) {
								if (depth == minimaxDepth) { // update MoveCommand
									std::random_device rd;
									std::mt19937 rng(rd());
									std::uniform_int_distribution<int> uni(0, 1); // depth is always max here...
									if (moveScore > bestValue || bestValue == -INF_SCORE || uni(rng) == 0) {
										command->setMove(move);
										command->setJokerTransform(JokerTransform(Position(-1, -1), (PieceType)-1));
									}
								}
								bestValue = moveScore;
							}

							// pruning
							a = max(a, moveScore);
							if (a >= b) {
								delete (gameCopy);
								return bestValue;
							}
						}
						else {
							for (Position jokerPos : jokerPositions) {
								PieceType jokerOriginType = getPieceType(game->getPieceAt(jokerPos)->getJokerRep());

								for (PieceType newType : jokerReps) {
									gameCopy->changeJoker(jokerPos, newType);
									moveScore = -minimax(gameCopy, depth - 1, -b, -a, -color, command, skipMinNodes, tryJokerPermutations);
									if (moveScore >= bestValue) {
										if (depth == minimaxDepth) { // update MoveCommand
											std::random_device rd;
											std::mt19937 rng(rd());
											std::uniform_int_distribution<int> uni(0, 1);
											if (moveScore > bestValue || bestValue == -INF_SCORE || uni(rng) == 0) {
												if (jokerPos == move.getFrom())
													jokerPos = move.getTo();

												command->setJokerTransform(JokerTransform(jokerPos, newType));
												command->setMove(move);
											}
										}
										bestValue = moveScore;
									}

									// pruning
									a = max(a, moveScore);
									if (a >= b) {
										delete (gameCopy);
										return bestValue;
									}

									gameCopy->changeJoker(jokerPos, jokerOriginType); // undo joker change
								}
							}
						}

						delete (gameCopy);
					}
				}
			}
		}
	}

	return bestValue;
}

std::pair<MoveCommand*, float> minimaxSuggestMove(GameBoard& game, int depth, bool skipMinNodes, bool tryJokerPermutations)
{
	playerNumber = (game.getCurrentPlayer() == 1 ? 1 : -1);
	minimaxDepth = depth;

	Position pos = Position(-1, -1);
	GameMove move = GameMove(pos, pos);
	JokerTransform jokerChange = JokerTransform(pos, Rock);
	MoveCommand* command = new MoveCommand(move, jokerChange);

	//int score = minimax(&game, MINIMAX_DEPTH, -INF_SCORE, INF_SCORE, 1, command);
	float score = minimax(&game, minimaxDepth, -INF_SCORE, INF_SCORE, (game.getCurrentPlayer() == 1 ? 1 : -1), command, skipMinNodes, tryJokerPermutations);

	return std::pair<MoveCommand*, float>(command, score);
}