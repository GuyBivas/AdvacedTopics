#include "FilePlayerAlgorithm.h"

template<class T>
vector<T> parseFile(ifstream& infile, function<T(string)> getCommandFunc)
{
	vector<T> res = {};
	string line;

	while (getline(infile, line)) {
		T command = getCommandFunc(line);

		if (command.getMessageType() != ParseOK) {
			cout << "Error: parse failed - " << line << endl;
			return res;
		}

		res.push_back(command);
	}

	return res;
}

void FilePlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
	string positionsPath;
	string movesPath;

	if (player == 1) {
		positionsPath = positionsPathPlayer1;
		movesPath = movesPathPlayer1;
	}
	else {
		positionsPath = positionsPathPlayer2;
		movesPath = movesPathPlayer2;
	}

	ifstream positionFile(positionsPath);
	ifstream movesFile(movesPath);

	if (!positionFile.good() || !movesFile.good())
	{
		cout << "Error opening files." << endl;
		return;
	}

	vector<PositioningCommand> positions = parseFile<PositioningCommand>(positionFile, parsePositioningCommand);
	moveCommands = parseFile<ParserMoveCommand>(movesFile, parseMoveCommand);

	positionFile.close();
	movesFile.close();

	for (auto command : positions) {
		Position p = command.getPos();
		Piece piece(command.getType(), p, player, command.getIsJoker());
		vectorToFill.push_back(make_unique<Piece>(piece));
	}
}

unique_ptr<JokerChange> FilePlayerAlgorithm::getJokerChange()
{
	if (index >= moveCommands.size() || moveCommands[index].getJokerTransform().getRep() == -1) return nullptr;
	return make_unique<JokerTransform>(moveCommands[index++].getJokerTransform());
}
