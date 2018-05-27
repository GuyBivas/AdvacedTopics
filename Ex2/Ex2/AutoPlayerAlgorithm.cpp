#include "AutoPlayerAlgorithm.h"
#include <iostream>

const std::vector<PieceType> jokerReprs = { Rock, Paper, Scissors };

Position flipHorizontal(Position pos)
{
	return Position(COLS + 1 - pos.getX(), pos.getY());
}

Position flipVertical(Position pos)
{
	return Position(pos.getX(), ROWS + 1 - pos.getY());
}

Position getRandomPos(map<Position, PieceType> positions) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(1, 10);

	int x = uni(rng);
	int y = uni(rng);
	Position res = Position(x, y);

	if (positions.find(res) == positions.end())
		return res;
	else
		return getRandomPos(positions);
}

void AutoPlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
	playerNum = player;
	playerBoard.setCurrPlayer(player);
	opponentBoard.setCurrPlayer(getOppositePlayer(player));

	// TODO: add flip diag and test which flip has the best results against order/random
	// TODO: put joker far from center so they will join the action when there is more knowledge
	map<Position, PieceType> positions = {
		{ Position(1, 8) , Flag },
		{ Position(2, 8), Bomb },
		{ Position(1, 7), Bomb },
		{ Position(3, 9), Scissors },
		{ Position(3, 7), Rock },
		{ Position(5, 4), Rock },
		{ Position(1, 9), Paper },
		{ Position(2, 5), Paper },
		{ Position(3, 4), Paper },
		{ Position(6, 5), Paper },
		{ Position(5, 9), Paper },
		{ Position(3, 6), Joker },
		{ Position(5, 7), Joker } };

	if (playerNum == 0)
	{
		positions = {};
		positions[getRandomPos(positions)] = Flag;
		positions[getRandomPos(positions)] = Bomb;
		positions[getRandomPos(positions)] = Bomb;
		positions[getRandomPos(positions)] = Scissors;
		positions[getRandomPos(positions)] = Rock;
		positions[getRandomPos(positions)] = Rock;
		positions[getRandomPos(positions)] = Paper;
		positions[getRandomPos(positions)] = Paper;
		positions[getRandomPos(positions)] = Paper;
		positions[getRandomPos(positions)] = Paper;
		positions[getRandomPos(positions)] = Paper;
		positions[getRandomPos(positions)] = Joker;
		positions[getRandomPos(positions)] = Joker;
	}

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, 1);

	bool horizontalFlip = (uni(rng) == 0);
	bool verticalFlip = (uni(rng) == 0);

	for (auto pair : positions)
	{
		bool isJoker = (pair.second == Joker);
		Position pos = pair.first;

		//if (horizontalFlip)
			if (playerNum == 1)
			pos = flipHorizontal(pos);

		//if (verticalFlip)
			if (playerNum == 1)
			pos = flipVertical(pos);

		vectorToFill.push_back(make_unique<Piece>((isJoker ? Scissors : pair.second), pos, playerNum, isJoker));
		playerBoard.setPos(pos, new AlgoPiece((isJoker ? Scissors : pair.second), pos, playerNum, isJoker, true));
	}
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights)
{
	GameBoard opponentInitialBoard = GameBoard();

	// place opponent's unknown pieces
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Position pos = Position(j, i);
			if (b.getPlayer(pos) == getOppositePlayer(playerNum)) {
				AlgoPiece* piece = new AlgoPiece((PieceType)-1, pos, getOppositePlayer(playerNum), false, false);
				opponentBoard.setPos(pos, piece);

				AlgoPiece* piece2 = new AlgoPiece((PieceType)-1, pos, getOppositePlayer(playerNum), false, false);
				opponentInitialBoard.setPos(pos, piece2);
			}
		}
	}

	// handle fights
	for (size_t i = 0; i < fights.size(); i++) {
		auto fight = fights[i].get();

		AlgoPiece* piece = new AlgoPiece((PieceType)-1, fight->getPosition(), getOppositePlayer(playerNum), false, false);
		opponentInitialBoard.setPos(fight->getPosition(), piece);

		PieceType enemyPiece = getPieceType(fight->getPiece(getOppositePlayer(playerNum)));
		if (fight->getWinner() == playerNum) {
			enemyPieceCount[enemyPiece]--;
			opponentBoard.setPos(fight->getPosition(), nullptr);
		}
		else if (fight->getWinner() == getOppositePlayer(playerNum)) {
			((AlgoPiece*)(opponentBoard.getPieceAt(fight->getPosition())))->setType(enemyPiece);
			((AlgoPiece*)(opponentBoard.getPieceAt(fight->getPosition())))->setIsKnown(true);
			playerBoard.setPos(fight->getPosition(), nullptr);
		}
		else { // draw
			enemyPieceCount[enemyPiece]--;
			playerBoard.setPos(fight->getPosition(), nullptr);
			opponentBoard.setPos(fight->getPosition(), nullptr);
		}
	}

	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Position pos = Position(j, i);
			AlgoPiece* p = (AlgoPiece*)opponentBoard.getPieceAt(pos);
			if (p != nullptr)
				p->setFlagChance(flagProbabilty(&opponentInitialBoard, p));
		}
	}
}

void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move& move)
{
	((AlgoPiece*)opponentBoard.getPieceAt(move.getFrom()))->setHasMoved();

	Position from = Position(move.getFrom().getX(), move.getFrom().getY());
	Position to = Position(move.getTo().getX(), move.getTo().getY());
	GameMove opMove = GameMove(from, to);

	opponentBoard.movePiece(opMove);
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo& fightInfo)
{
	PieceType opPieceType = getPieceType(fightInfo.getPiece(getOppositePlayer(playerNum)));

	if (fightInfo.getWinner() == 0) { // draw
		enemyPieceCount[opPieceType]--;
		playerBoard.setPos(fightInfo.getPosition(), nullptr);
		opponentBoard.setPos(fightInfo.getPosition(), nullptr);
	}
	else if (fightInfo.getWinner() == playerNum) {
		enemyPieceCount[opPieceType]--;
		opponentBoard.setPos(fightInfo.getPosition(), nullptr);
	}
	else {
		playerBoard.setPos(fightInfo.getPosition(), nullptr);
		((AlgoPiece*)(opponentBoard.getPieceAt(fightInfo.getPosition())))->setType(opPieceType);
		((AlgoPiece*)(opponentBoard.getPieceAt(fightInfo.getPosition())))->setIsKnown(true);
	}
}

int AutoPlayerAlgorithm::calcMinimaxDepth()
{
	//int depth = 2;
	int depth = 1;

	GameBoard board = GameBoard();
	guessOpponentPieces(board);

	auto isMoving = [](Piece* p) { return p->getIsMovingPiece(); };
	auto isJoker = [](Piece* p) { return p->getIsJoker(); };

	int movingCount1 = playerBoard.countPiecesIf(isMoving);
	int jokersCount1 = playerBoard.countPiecesIf(isJoker);
	int movingCount2 = board.countPiecesIf(([&](Piece* p) { return isMoving(p) && p->getPlayerNum() != playerNum; }));
	//int jokersCount2 = board.countPiecesIf(([&](Piece* p) { return isJoker(p) && p->getPlayerNum() != playerNum; })); // this is 0

	int minimaxHardness = (movingCount1 * (jokersCount1 + 1)) * (movingCount2 * (jokersCount1 + 1)); // (4^moving)*(3^jokers) is better

	if (playerNum == 0) {
		if (minimaxHardness <= 8)
			depth = 3;
		else if (minimaxHardness <= 48)
			depth = 3;
		else if (minimaxHardness <= 110)
			depth = 3;
		else if (minimaxHardness <= 900)
			depth = 1;
	}
	else
	{
		if (minimaxHardness <= 6)
			depth = 3;
		else if (minimaxHardness <= 48)
			depth = 3;
		else if (minimaxHardness <= 100)
			depth = 3;
		else if (minimaxHardness <= 900)
			depth = 1;
	}

	//if (minimaxHardness <= 6)
	//	depth = 7;
	//else if (minimaxHardness <= 16)
	//	depth = 5;
	//else if (minimaxHardness <= 64)
	//	depth = 3;
	//else if (minimaxHardness <= 900)
	//	depth = 1;

	//if (minimaxHardness <= 16)
	//	depth = 7;
	//else if (minimaxHardness <= 84)
	//	depth = 5;
	//else if (minimaxHardness <= 640)
	//	depth = 3;
	//else if (minimaxHardness <= 900)
	//	depth = 2;

	//if (pieceCount <= 17)
	//	depth = 3;

	//if (pieceCount <= 10)
	//	depth = 5;

	//if (pieceCount <= 5)
	//	depth = 7;

	return depth;
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	int depth = 1;// calcMinimaxDepth();
	depth = calcMinimaxDepth();
	//depth = (playerNum == 1 ? calcMinimaxDepth() : 1);
	//depth = (playerNum == 1 ? 1 : 1);

	unordered_map<MoveCommand, float> movesFrequency = {};
	for (int i = 0; i < (playerNum == 0 ? 60 : depth == 1 ? 60 : 10); i++) {
		GameBoard board = GameBoard();
		board.setCurrPlayer(playerNum);
		guessOpponentPieces(board);

		pair<MoveCommand*, float> moveCmd = minimaxSuggestMove(board, depth);
		moveCmd.first->setJokerTransform(JokerTransform(Position(-1, -1), (PieceType)-1));

		float moveVal = 1;
		if (moveCmd.second == WIN_SCORE) { // move that led to game win gets 1.5x in frequncy val
			moveVal = 1.4;
		}
		if (moveCmd.second == -WIN_SCORE) {
			//moveVal = 0.1;

			moveCmd = minimaxSuggestMove(board, 1);
			moveCmd.first->setJokerTransform(JokerTransform(Position(-1, -1), (PieceType)-1));
			moveVal = 1;
			if (moveCmd.second == WIN_SCORE) { // move that led to game win gets 1.5x in frequncy val
				moveVal = 1.4;
			}
		}

		movesFrequency[*(moveCmd.first)] += moveVal;
		delete(moveCmd.first);
	}

	float maxFrequency = movesFrequency.begin()->second;
	MoveCommand mostFrequentMove = movesFrequency.begin()->first;
	for (auto move : movesFrequency) {
		if (move.second > maxFrequency) {
			mostFrequentMove = move.first;
			maxFrequency = move.second;
		}
	}

	//lastMoveJokerChange = mostFrequentMove.getJokerTransform();
	GameMove move = mostFrequentMove.getMove();

	playerBoard.movePiece(move);

	unique_ptr<GameMove> res = make_unique<GameMove>(move);

	return res;
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	if (playerNum == 0)
	{
		return nullptr;

		if (lastMoveJokerChange.getRep() == -1)
			return nullptr;

		Piece* piece = playerBoard[lastMoveJokerChange.getJokerChangePosition()];
		if (piece != nullptr && piece->getIsJoker()) {
			if (piece->getJokerRep() != lastMoveJokerChange.getJokerNewRep()) {
				playerBoard.transformJoker(lastMoveJokerChange.getJokerChangePosition(), lastMoveJokerChange.getRep());
				return make_unique<JokerTransform>(lastMoveJokerChange);
			}
		}

		return nullptr;
	}

	unordered_map<MoveCommand, float> movesFrequency = {};
	vector<Position> jokerPositions = playerBoard.getPlayersJokersPos();
	int depth = (playerNum == 1 ? 3 : 1);

	if (jokerPositions.empty())
		return nullptr;

	for (int i = 0; i < (playerNum == 0 ? 20 : 20); i++) {
		GameBoard board = GameBoard();
		board.setCurrPlayer(playerNum);
		guessOpponentPieces(board, true);

		for (Position jokerPos : jokerPositions) {
			for (PieceType newType : jokerReprs) {
				board.changeJoker(jokerPos, newType);

				pair<MoveCommand*, float> moveCmd = minimaxSuggestMove(board, (i < 45 ? 1 : 1), false, false);

				GameMove move = GameMove(Position(-1, -1), Position(-1, -1));
				JokerTransform trans = JokerTransform(jokerPos, newType);
				MoveCommand command = MoveCommand(move, trans);

				movesFrequency[command] += moveCmd.second;
				delete(moveCmd.first);
			}
		}
	}

	float maxFrequency = movesFrequency.begin()->second;
	MoveCommand mostFrequentMove = movesFrequency.begin()->first;

	for (auto move : movesFrequency) {
		if (move.second == maxFrequency) {
			cout << "";
		}
		if (move.second > maxFrequency) {

			mostFrequentMove = move.first;
			maxFrequency = move.second;
		}
	}

	JokerTransform transform = mostFrequentMove.getJokerTransform();

	//if (playerNum == 2)
	//	transform.setRep(Scissors);

	Piece* piece = playerBoard[transform.getJokerChangePosition()];
	if (piece->getJokerRep() != transform.getJokerNewRep()) {
		playerBoard.transformJoker(transform.getJokerChangePosition(), transform.getRep());
		return make_unique<JokerTransform>(transform);
	}
	else {
		return nullptr;
	}
}

//unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
//{
//	if (lastMoveJokerChange.getRep() == -1)
//		return nullptr;
//
//	Piece* piece = playerBoard[lastMoveJokerChange.getJokerChangePosition()];
//	if (piece->getIsJoker()) {
//		if (piece->getJokerRep() != lastMoveJokerChange.getJokerNewRep()) {
//			playerBoard.transformJoker(lastMoveJokerChange.getJokerChangePosition(), lastMoveJokerChange.getRep());
//			return make_unique<JokerTransform>(lastMoveJokerChange);
//		}
//	}
//
//	return nullptr;
//}

AlgoPiece* chooseWithProbability(map<AlgoPiece*, int> map)
{
	int sum = 0;
	for (auto pair : map) {
		sum += pair.second;
	}

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, sum); // guaranteed unbiased
	int value = uni(rng); // draw value

	int tempVal = 0;
	for (auto pair : map) {
		tempVal += pair.second;
		if (tempVal >= value)
			return pair.first;
	}

	return nullptr; // should not happen
}

PieceType AutoPlayerAlgorithm::choosePieceTypeProbabilty() const
{
	map<PieceType, double> map = {};

	map[Rock] = 1 / (enemyPieceCount.at(Rock) + 1 + JOKER_COUNT);
	map[Paper] = 1 / (enemyPieceCount.at(Paper) + 1 + JOKER_COUNT);
	map[Scissors] = 1 / (enemyPieceCount.at(Scissors) + 1 + JOKER_COUNT);
	double sum = map[Rock] + map[Paper] + map[Scissors];

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_real_distribution<> dis(0, sum); // guaranteed unbiased
	double value = dis(rng); // draw value

	double tempVal = 0;
	for (auto pair : map) {
		tempVal += pair.second;
		if (tempVal >= value)
			return pair.first;
	}

	return (PieceType)-1; // should not happen
}

void AutoPlayerAlgorithm::guessOpponentPiecesByType(GameBoard& toFill, PieceType type, function<bool(AlgoPiece*)> condition, function<int(AlgoPiece*)> probabilty) const
{
	for (int i = 0; i < enemyPieceCount.at(type); i++) {
		map<AlgoPiece*, int> options = {};

		for (int col = 1; col <= COLS; col++) {
			for (int row = 1; row <= ROWS; row++) {
				AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
				if (piece != nullptr && toFill.getPieceAt(Position(col, row)) == nullptr && condition(piece)) {
					options[piece] = probabilty(piece);
				}
			}
		}

		if (options.size() == 0)
			return;

		AlgoPiece* chosen = chooseWithProbability(options);
		AlgoPiece* copy = copyAlgoPiece(chosen);
		copy->setType(type);
		toFill.putPiece(copy);
	}
}

int flagProbabilty(const GameBoard* opponentBoard, AlgoPiece* piece)
{
	Position pos = (Position)(piece->getPosition());
	int distanceFromCenter = pos.distanceFromCenter();
	bool isOnSide = pos.isOnSide();

	auto isAdjacent = [piece](Piece* p) { return p->distance(*piece) == 1; };
	int adjacentCount = opponentBoard->countPiecesIf(isAdjacent);

	return distanceFromCenter * 1 + isOnSide * 10 * 1 + adjacentCount * 15;
}

void AutoPlayerAlgorithm::guessOpponentPieces(GameBoard& toFill, bool onlyJokers) const
{
	// fill known pieces:
	for (int col = 1; col <= COLS; col++) {
		for (int row = 1; row <= ROWS; row++) {
			Position pos = Position(col, row);

			AlgoPiece* enemyPiece = (AlgoPiece*)opponentBoard.getPieceAt(pos);
			if (enemyPiece != nullptr && enemyPiece->getIsKnown()) {
				AlgoPiece* copy = copyAlgoPiece(enemyPiece);
				toFill.putPiece(copy);
			}

			AlgoPiece* playerPiece = (AlgoPiece*)playerBoard.getPieceAt(pos);
			if (playerPiece != nullptr && (!onlyJokers || playerPiece->getIsJoker() || playerPiece->getType() == Flag)) {
				AlgoPiece* copy = copyAlgoPiece(playerPiece);
				toFill.putPiece(copy);
			}
		}
	}

	// piece with bigger distance from center has higher probabilty to be flag
	//auto countAdjacent = [this](AlgoPiece* p) {  };
	auto flagProb2 = [this](AlgoPiece* p) { return ((Position)p->getPosition()).distanceFromCenter()/* + (((Position)p->getPosition()).isOnSide() ? 5 : 0)*/; };

	auto flagProb = [this](AlgoPiece* p) { return flagProbabilty(&opponentBoard, p); };
	//auto flagProb = [this](AlgoPiece* p) { return flagProbabilty1(p, opponentBoard); };
	//auto flagProb = [this](AlgoPiece* p) { return flagProbabilty1(p); };
	auto uniformProb = [](AlgoPiece* p) { if (p == nullptr) return 1; else return 1; };
	auto checkNotMoved = [](AlgoPiece* p) { return p->getHasMoved() == false; };
	auto emptyCondition = [](AlgoPiece* p) {  if (p == nullptr) return true; else return true; };
	auto conditionFunc = [](AlgoPiece* p) { return p->getHasMoved() == false; };

	if (playerNum == 0)
		guessOpponentPiecesByType(toFill, Flag, checkNotMoved, uniformProb);
	else
		guessOpponentPiecesByType(toFill, Flag, checkNotMoved, &(AlgoPiece::getFlagChance));

	guessOpponentPiecesByType(toFill, Bomb, checkNotMoved, uniformProb);
	guessOpponentPiecesByType(toFill, Rock, emptyCondition, uniformProb);
	guessOpponentPiecesByType(toFill, Paper, emptyCondition, uniformProb);
	guessOpponentPiecesByType(toFill, Scissors, emptyCondition, uniformProb);

	//auto conditionFunc = [](AlgoPiece* p) { return p->getHasMoved() == false; };
	//countPiecesScore(conditionFunc);

	// guess all remaining pieces
	vector<AlgoPiece*> remainingPieces;
	for (int col = 1; col <= COLS; col++) {
		for (int row = 1; row <= ROWS; row++) {
			AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
			if (piece != nullptr && toFill.getPieceAt(Position(col, row)) == nullptr) {
				remainingPieces.push_back(piece);
			}
		}
	}

	// fill last pieces by probabilty of occurencs (since joker can be any piece)
	for (size_t i = 0; i < remainingPieces.size(); i++)
	{
		AlgoPiece* piece = remainingPieces[i];
		AlgoPiece* copy = copyAlgoPiece(piece);
		copy->setType(choosePieceTypeProbabilty());
		toFill.putPiece(copy);
	}
}

AlgoPiece* copyAlgoPiece(AlgoPiece* piece)
{
	if (piece == nullptr)
		return nullptr;

	return new AlgoPiece(*piece);
}