#include "AutoPlayerAlgorithm.h"

Position flipHorizontal(Position pos)
{
	return Position(COLS + 1 - pos.getX(), pos.getY());
}

Position flipVertical(Position pos)
{
	return Position(pos.getX(), ROWS + 1 - pos.getY());
}

void AutoPlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
	playerNum = player;

	map<Position, PieceType> positions = {
		{ Position(1, 8) , Flag },
		{ Position(2,8), Bomb },
		{ Position(1,9), Bomb },
		{ Position(3,9), Scissors },
		{ Position(3,7), Rock },
		{ Position(5,4), Rock },
		{ Position(1,7), Paper },
		{ Position(2,5), Paper },
		{ Position(3,4), Paper },
		{ Position(6,5), Paper },
		{ Position(5,9), Paper },
		{ Position(3,6), Joker },
		{ Position(5,7), Joker }
	};

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, 1); // guaranteed unbiased

	bool horizontalFlip = (uni(rng) == 0);
	bool verticalFlip = (uni(rng) == 0);

	for (auto pair : positions)
	{
		bool isJoker = (pair.second == Joker);
		Position pos = pair.first;

		if (horizontalFlip)
			pos = flipHorizontal(pos);

		if (verticalFlip)
			pos = flipVertical(pos);

		vectorToFill.push_back(make_unique<Piece>((isJoker ? Scissors : pair.second), pos, playerNum, isJoker));
		playerBoard.setPos(pos, new AlgoPiece((isJoker ? Scissors : pair.second), pos, playerNum, isJoker, true));
	}
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights)
{
	// place oponent's unknown pieces
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
			opponentBoard.setPos(fight->getPosition(), NULL);
		}
		else if (fight->getWinner() == getOppositePlayer(playerNum)) {
			((AlgoPiece*)(opponentBoard.getPieceAt(fight->getPosition())))->setType(enemyPiece);
			((AlgoPiece*)(opponentBoard.getPieceAt(fight->getPosition())))->setIsKnown(true);
			playerBoard.setPos(fight->getPosition(), NULL);
		}
		else { // draw
			enemyPieceCount[enemyPiece]--;
			playerBoard.setPos(fight->getPosition(), NULL);
			opponentBoard.setPos(fight->getPosition(), NULL);
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
		playerBoard.setPos(fightInfo.getPosition(), NULL);
		opponentBoard.setPos(fightInfo.getPosition(), NULL);
	}
	else if (fightInfo.getWinner() == playerNum) {
		enemyPieceCount[opPieceType]--;
		opponentBoard.setPos(fightInfo.getPosition(), NULL);
	}
	else {
		playerBoard.setPos(fightInfo.getPosition(), NULL);
		((AlgoPiece*)(opponentBoard.getPieceAt(fightInfo.getPosition())))->setType(opPieceType);
		((AlgoPiece*)(opponentBoard.getPieceAt(fightInfo.getPosition())))->setIsKnown(true);
	}
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	unordered_map<MoveCommand, float> movesFrequency = {};

	for (int i = 0; i < GUESS_AMOUNT; i++) {
		GameBoard board = GameBoard();
		guessOpponentPieces(board);
		pair<MoveCommand, int> moveCmd = minimaxSuggestMove(board);
		float moveVal = 1;
		if (moveCmd.second == WIN_SCORE) { // move that led to game win gets 1.5x in frequncy val
			moveVal = 1.5;
		}

		movesFrequency[moveCmd.first] += moveVal;
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
	return res;
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	Piece* piece = playerBoard[lastMoveJokerChange.getJokerChangePosition()];
	if (piece->getIsJoker()) {
		if (piece->getJokerRep() != lastMoveJokerChange.getJokerNewRep()) {
			playerBoard.transformJoker(lastMoveJokerChange.getJokerChangePosition(), lastMoveJokerChange.getRep());
			return make_unique<JokerTransform>(lastMoveJokerChange);
		}
	}
	
	return nullptr;
}

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
		if (pair.second >= value)
			return pair.first;
	}

	return NULL; // should not happen
}

PieceType AutoPlayerAlgorithm::choosePieceTypeProbabilty()
{
	map<PieceType, double> map = {};

	map[Rock] = 1 / (enemyPieceCount[Rock] + 1 + JOKER_COUNT);
	map[Paper] = 1 / (enemyPieceCount[Paper] + 1 + JOKER_COUNT);
	map[Scissors] = 1 / (enemyPieceCount[Scissors] + 1 + JOKER_COUNT);
	double sum = map[Rock] + map[Paper] + map[Scissors];

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_real_distribution<> dis(0, sum); // guaranteed unbiased
	double value = dis(rng); // draw value

	double tempVal = 0;
	for (auto pair : map) {
		tempVal += pair.second;
		if (pair.second >= value)
			return pair.first;
	}

	return (PieceType)-1; // should not happen
}

void AutoPlayerAlgorithm::guessOpponentPiecesByType(GameBoard& toFill, PieceType type, function<bool(AlgoPiece*)> condition, function<int(AlgoPiece*)> probabilty)
{
	for (int i = 0; i < enemyPieceCount[type]; i++) {
		map<AlgoPiece*, int> options = {};

		for (int col = 1; col <= COLS; col++) {
			for (int row = 1; row <= ROWS; row++) {
				AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
				if (piece != NULL && toFill.getPieceAt(Position(col, row)) == NULL && condition(piece)) {
					options[piece] = probabilty(piece);
				}
			}
		}

		if (options.size() == 0)
			return;

		AlgoPiece* chosen = chooseWithProbability(options);
		Piece* copy = new Piece(*chosen);
		copy->setType(type);
		toFill.putPiece(copy);
	}
}

void AutoPlayerAlgorithm::guessOpponentPieces(GameBoard& toFill)
{
	// fill known pieces:
	for (int col = 1; col <= COLS; col++) {
		for (int row = 1; row <= ROWS; row++) {
			AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
			if (piece != NULL && piece->getIsKnown()) {
				Piece* copy = new Piece(*piece);
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
			if (piece != NULL && toFill.getPieceAt(Position(col, row)) == NULL) {
				remainingPieces.push_back(piece);
			}
		}
	}

	// fill last pieces by probabilty of occurencs (since joker can be any piece)
	for (size_t i = 0; i < remainingPieces.size(); i++)
	{
		AlgoPiece* piece = remainingPieces[i];
		Piece* copy = new Piece(*piece);
		copy->setType(choosePieceTypeProbabilty());
		toFill.putPiece(copy);
	}
}
