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
		PieceType enemyPiece = getPieceType(fight->getOpponentPiece());
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

void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
	((AlgoPiece*)opponentBoard.getPieceAt(move.getFrom()))->setHasMoved();

	Position from = Position(move.getFrom().getX(), move.getFrom().getY());
	Position to = Position(move.getTo().getX(), move.getTo().getY());
	GameMove opMove = GameMove(from, to);

	opponentBoard.movePiece(opMove);
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
	PieceType opPieceType = getPieceType(fightInfo.getOpponentPiece());

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

	map[Rock] = 1 / (enemyPieceCount[Rock] + 1);
	map[Paper] = 1 / (enemyPieceCount[Paper] + 1);
	map[Scissors] = 1 / (enemyPieceCount[Scissors] + 1);
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

void AutoPlayerAlgorithm::guessOpponentPieces(GameBoard& toFill)
{
	// fill known pieces:
	for (int col = 1; col <= COLS; col++) {
		for (int row = 1; row <= ROWS; row++) {
			AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
			if (piece != NULL && piece->getIsKnown()) {
				toFill.setPos(piece->getPosition(), piece);
			}
		}
	}

	// ----------------------------

	// Guess flags and bombs: 
	map<AlgoPiece*, int> flagBombChances = {};
	for (int col = 1; col <= COLS; col++) {
		for (int row = 1; row <= ROWS; row++) {
			AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
			if (piece != NULL && !piece->getHasMoved()) {
				flagBombChances[piece] = (Position(ROWS / 2, COLS / 2) - piece->getPosition()).magnitude(); // piece with bigger distance from center has higher probabilty to be a flag
			}
		}
	}

	// ----------------------------

	// guess flag - currently handles one flag only
	AlgoPiece* flag = chooseWithProbability(flagBombChances);
	toFill.setPos(flag->getPosition(), flag);

	// ----------------------------

	// guess bombs ----------------------------
	for (int i = 0; i < enemyPieceCount[Bomb]; i++) {
		map<AlgoPiece*, int> bombs = {};

		for (int col = 1; col <= COLS; col++) {
			for (int row = 1; row <= ROWS; row++) {
				AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
				if (piece != NULL && !piece->getHasMoved() && toFill.getPieceAt(Position(col, row)) == NULL) {
					bombs[piece] = 1; // piece with  bigger distance from center has higher probabilty
				}
			}
		}

		AlgoPiece* bomb = chooseWithProbability(bombs);
		toFill.setPos(bomb->getPosition(), bomb);
	}

	// ----------------------------

	// guess all other pieces ----------------------------
	vector<AlgoPiece*> remainingPieces;
	for (int col = 1; col <= COLS; col++) {
		for (int row = 1; row <= ROWS; row++) {
			AlgoPiece* piece = (AlgoPiece*)opponentBoard.getPieceAt(Position(col, row));
			if (piece != NULL && toFill.getPieceAt(Position(col, row)) == NULL) {
				remainingPieces.push_back(piece);
			}
		}
	}

	auto rng = std::default_random_engine{};
	shuffle(begin(remainingPieces), end(remainingPieces), rng);

	size_t i = 0;
	for (auto pair : enemyPieceCount) {
		if (pair.first == Bomb || pair.first == Flag) continue;
		for (int j = 0; j < pair.second - JOKER_COUNT; j++) {
			toFill.setPos(remainingPieces[i]->getPosition(), remainingPieces[i]);
			i++;
		}
	}

	while (i < remainingPieces.size()) { // fill last pieces by probabilty of occurencs (since joker can be any piece)
		AlgoPiece* piece = remainingPieces[i];
		piece->setType(choosePieceTypeProbabilty());
		toFill.setPos(piece->getPosition(), piece);
	}
	// ----------------------------
}