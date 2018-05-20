#include "Minimax.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

int playerNumber;

const std::vector<Position> directions = { Position(-1,0), Position(1,0), Position(0,1), Position(0,-1) };
const std::vector<PieceType> jokerReps = { Scissors, Paper, Rock };

vector<Position> getPlayersJokersPos(GameBoard* game) {
	vector<Position> positions = {};

	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Piece* piece = game->getPieceAt(j, i);

			if ((piece != nullptr) && (piece->getPlayerNum() == game->getCurrentPlayer()) && (piece->getIsJoker())) {
				Position pos = Position(j, i);
				positions.push_back(pos);
			}
		}
	}

	return positions;
}

int getPieceScore(Piece* piece)
{
	int score;

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
		}
	}

	totalX1 /= pieceCount1;
	totalY1 /= pieceCount1;
	totalX2 /= pieceCount2;
	totalY2 /= pieceCount2;

	float avgDistance = abs(totalX1 - totalX2) + abs(totalY1 - totalY2);

	return score - avgDistance*playerNumber;
}

float getGameScore(GameBoard* game)
{
	float score = 0;
	//LOGFILE << "Entered minimax getGameScore, player " << game->getCurrentPlayer() << endl;
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

	//LOGFILE << "Exited minimax getGameScore, player " << game->getCurrentPlayer() << endl;

	return score;
}

float minimax(GameBoard* game, int depth, float a, float b, int color, MoveCommand* command)
{
	//LOGFILE << "Entered Minimax Level " << depth << endl;

	if (depth == 0 || (game->isGameOver() && depth != MINIMAX_DEPTH)) {
		return getGameScore(game) * color;
	}

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
						//LOGFILE << "Minimax Level " << depth << " Entered if statement, before copy" << endl;
						GameBoard* gameCopy = new GameBoard(*game);
						GameMove move = GameMove(pos, to);
						gameCopy->move(move);
						gameCopy->setCurrPlayer(gameCopy->getOtherPlayer());
						//LOGFILE << "Minimax Level " << depth << " Entered if statement, after copy" << endl;
						// change joker
						vector<Position> jokerPositions = {};
						if (depth == MINIMAX_DEPTH)
							jokerPositions = getPlayersJokersPos(game); 
						if (jokerPositions.empty()) {
							moveScore = -minimax(gameCopy, depth - 1, -b, -a, -color, command);
							if (moveScore >= bestValue) {
								std::random_device rd;
								std::mt19937 rng(rd());
								std::uniform_int_distribution<int> uni(0, 1);
								if (moveScore > bestValue || bestValue == -INF_SCORE || uni(rng) == 0) {
									if (depth == MINIMAX_DEPTH) { // update MoveCommand
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
									moveScore = -minimax(gameCopy, depth - 1, -b, -a, -color, command);
									if (moveScore >= bestValue) {
										if (depth == MINIMAX_DEPTH) { // update MoveCommand
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

std::pair<MoveCommand*, float> minimaxSuggestMove(GameBoard& game)
{
	//LOGFILE << "Entered minimaxSuggestMove" << endl;

	playerNumber = (game.getCurrentPlayer() == 1 ? 1 : -1);

	Position pos = Position(-1, -1);
	GameMove move = GameMove(pos, pos);
	JokerTransform jokerChange = JokerTransform(pos, Rock);
	MoveCommand* command = new MoveCommand(move, jokerChange);

	//int score = minimax(&game, MINIMAX_DEPTH, -INF_SCORE, INF_SCORE, 1, command);
	float score = minimax(&game, MINIMAX_DEPTH, -INF_SCORE, INF_SCORE, (game.getCurrentPlayer() == 1 ? 1 : -1), command);

	//LOGFILE << "Exited minimaxSuggestMove" << endl;

	return std::pair<MoveCommand*, float>(command, score);
}