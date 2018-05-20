#include "AutoPlayerAlgorithm.h"
#include <iostream>

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

	map<Position, PieceType> positions = {
		{ Position(1, 8) , Flag },
		{ Position(2, 8), Bomb },
		{ Position(1, 9), Bomb },
		{ Position(3, 9), Scissors },
		{ Position(3, 7), Rock },
		{ Position(5, 4), Rock },
		{ Position(1, 7), Paper },
		{ Position(2, 5), Paper },
		{ Position(3, 4), Paper },
		{ Position(6, 5), Paper },
		{ Position(5, 9), Paper },
		{ Position(3, 6), Joker },
		{ Position(5, 7), Joker } };

	//if (playerNum == 2)
	//{
	//	positions = {};
	//	positions[getRandomPos(positions)] = Flag;
	//	positions[getRandomPos(positions)] = Bomb;
	//	positions[getRandomPos(positions)] = Bomb;
	//	positions[getRandomPos(positions)] = Scissors;
	//	positions[getRandomPos(positions)] = Rock;
	//	positions[getRandomPos(positions)] = Rock;
	//	positions[getRandomPos(positions)] = Paper;
	//	positions[getRandomPos(positions)] = Paper;
	//	positions[getRandomPos(positions)] = Paper;
	//	positions[getRandomPos(positions)] = Paper;
	//	positions[getRandomPos(positions)] = Paper;
	//	positions[getRandomPos(positions)] = Joker;
	//	positions[getRandomPos(positions)] = Joker;
	//}

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, 1); // guaranteed unbiased

	bool horizontalFlip = (uni(rng) == 0);
	bool verticalFlip = (uni(rng) == 0);

	for (auto pair : positions)
	{
		bool isJoker = (pair.second == Joker);
		Position pos = pair.first;

		//if (horizontalFlip)
		if (playerNum == 2)
			pos = flipHorizontal(pos);

		//if (verticalFlip)
		if (playerNum == 2)
			pos = flipVertical(pos);

		vectorToFill.push_back(make_unique<Piece>((isJoker ? Scissors : pair.second), pos, playerNum, isJoker));
		playerBoard.setPos(pos, new AlgoPiece((isJoker ? Scissors : pair.second), pos, playerNum, isJoker, true));
	}
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights)
{
	// place opponent's unknown pieces
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Position pos = Position(j, i);
			if (b.getPlayer(pos) == getOppositePlayer(playerNum)) {
				AlgoPiece* piece = new AlgoPiece((PieceType)-1, pos, getOppositePlayer(playerNum), false, false);
				opponentBoard.setPos(pos, piece);
			}
		}
	}

	// handle fights
	for (size_t i = 0; i < fights.size(); i++) {
		auto fight = fights[i].get();
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
		if (fightInfo.getPosition() == lastMoveJokerChange.getJokerChangePosition()) {
			lastMoveJokerChange.setRep((PieceType)-1);
		}
	}
	else if (fightInfo.getWinner() == playerNum) {
		enemyPieceCount[opPieceType]--;
		opponentBoard.setPos(fightInfo.getPosition(), nullptr);
	}
	else {
		playerBoard.setPos(fightInfo.getPosition(), nullptr);
		((AlgoPiece*)(opponentBoard.getPieceAt(fightInfo.getPosition())))->setType(opPieceType);
		((AlgoPiece*)(opponentBoard.getPieceAt(fightInfo.getPosition())))->setIsKnown(true);
		if (fightInfo.getPosition() == lastMoveJokerChange.getJokerChangePosition()) {
			lastMoveJokerChange.setRep((PieceType)-1);
		}
	}
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	//LOGFILE << "Entered AutoPlayerAlgorithm getMove, player " << playerNum << endl;
	
	unordered_map<MoveCommand, float> movesFrequency = {};
	for (int i = 0; i < GUESS_AMOUNT; i++) {
		GameBoard board = GameBoard();
		board.setCurrPlayer(playerNum);
		guessOpponentPieces(board);
		pair<MoveCommand*, float> moveCmd = minimaxSuggestMove(board);
		float moveVal = 1;
		if (moveCmd.second == WIN_SCORE) { // move that led to game win gets 1.5x in frequncy val
			moveVal = 1.5;
		}

		movesFrequency[*(moveCmd.first)] += moveVal;
		delete(moveCmd.first);
	}

	float maxFrequency = movesFrequency.begin()->second;
	MoveCommand mostFrequentMove = movesFrequency.begin()->first;
	for (auto move : movesFrequency) {
		if (move.second > maxFrequency) {
			mostFrequentMove = move.first;
		}
	}

	lastMoveJokerChange = mostFrequentMove.getJokerTransform();
	GameMove move = mostFrequentMove.getMove();

	playerBoard.movePiece(move);

	unique_ptr<GameMove> res = make_unique<GameMove>(move);

	//LOGFILE << "Exited AutoPlayerAlgorithm getMove, player " << playerNum << endl;

	return res;
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	//LOGFILE << "Entered AutoPlayerAlgorithm getJokerChange, player " << playerNum << endl;
	
	if (lastMoveJokerChange.getRep() == -1)
		return nullptr;

	Piece* piece = playerBoard[lastMoveJokerChange.getJokerChangePosition()];
	if (piece->getIsJoker()) {
		if (piece->getJokerRep() != lastMoveJokerChange.getJokerNewRep()) {
			playerBoard.transformJoker(lastMoveJokerChange.getJokerChangePosition(), lastMoveJokerChange.getRep());
			return make_unique<JokerTransform>(lastMoveJokerChange);
		}
	}

	//LOGFILE << "Exited AutoPlayerAlgorithm getJokerChange, player " << playerNum << endl;

	return nullptr;
}

AlgoPiece* chooseWithProbability(map<AlgoPiece*, int> map)
{
	//LOGFILE << "Entered AutoPlayerAlgorithm chooseWithProbability" << endl;
	
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

	//LOGFILE << "Exited AutoPlayerAlgorithm chooseWithProbability" << endl;

	return nullptr; // should not happen
}

PieceType AutoPlayerAlgorithm::choosePieceTypeProbabilty() const
{
	//LOGFILE << "Entered AutoPlayerAlgorithm choosePieceTypeProbabilty" << endl;
	
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

	//LOGFILE << "Exited AutoPlayerAlgorithm choosePieceTypeProbabilty" << endl;

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

void AutoPlayerAlgorithm::guessOpponentPieces(GameBoard& toFill) const
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
			if (playerPiece != nullptr) {
				AlgoPiece* copy = copyAlgoPiece(playerPiece);
				toFill.putPiece(copy);
			}
		}
	}

	// piece with bigger distance from center has higher probabilty to be flag
	auto flagProb = [](AlgoPiece* p) { return (Position(ROWS / 2, COLS / 2) - p->getPosition()).magnitude(); };
	auto uniformProb = [](AlgoPiece* p) { return 1; };
	auto checkNotMoved = [](AlgoPiece* p) { return p->getHasMoved() == false; };
	auto emptyCondition = [](AlgoPiece* p) { return true; };

	guessOpponentPiecesByType(toFill, Flag, checkNotMoved, flagProb);
	guessOpponentPiecesByType(toFill, Bomb, checkNotMoved, uniformProb);
	guessOpponentPiecesByType(toFill, Rock, emptyCondition, uniformProb);
	guessOpponentPiecesByType(toFill, Paper, emptyCondition, uniformProb);
	guessOpponentPiecesByType(toFill, Scissors, emptyCondition, uniformProb);

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