#include "Parser.h"


vector<string> split(string str, char delimiter) {
	vector<string> splittedString = {};
	size_t pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		if (pos != 0)
			splittedString.push_back(str.substr(0, pos));

		str.erase(0, pos+1); // delete the substring and the extra char of the delimiter
	}

	if (!str.empty())
		splittedString.push_back(str);

	return splittedString;
}


PositioningCommand parsePositioningCommand(string line) {
	ParserMessageType messageType = ParseInvalidCommand;
	vector<string> splitted = split(line, ' ');
	if (splitted.size() == 0 || splitted[0].length() != 1)
		return PositioningCommand(messageType);

	PieceType piece = getPieceType(splitted[0][0]);
	if (piece == -1)
		return PositioningCommand(messageType);

	else if (splitted.size() == 3) // normal piece
	{
		if (atoi(splitted[1].c_str()) != 0 && atoi(splitted[2].c_str()) != 0)
		{
			Position pos = Position(atoi(splitted[1].c_str()), atoi(splitted[2].c_str()));

			if (!pos.isInBoard())
			{
				messageType = ParseOutOfBounds;
				return PositioningCommand(messageType);
			}
			else if (piece == Joker)
				return PositioningCommand(messageType);
			else
			{
				messageType = ParseOK;
				return PositioningCommand(messageType, piece, pos, false);
			}
		}
	}
	else if (splitted.size() == 4) // joker piece
	{
		if (!((splitted[3].length() == 1) || (splitted[3].length() == 2 && (splitted[3][1] == '\n' || splitted[3][1] == '\r'))))
			return PositioningCommand(messageType);

		PieceType secondPiece = getPieceType(splitted[3][0]);
		if (piece == Joker && atoi(splitted[1].c_str()) != 0 && atoi(splitted[2].c_str()) != 0 && secondPiece != -1)
		{
			Position pos = Position(atoi(splitted[1].c_str()), atoi(splitted[2].c_str()));
			if (!pos.isInBoard())
			{
				messageType = ParseOutOfBounds;
				return PositioningCommand(messageType);
			}
			else if (secondPiece == Joker || secondPiece == Flag)
			{
				messageType = ParseInvalidJokerRep;
				return PositioningCommand(messageType);
			}
			else
			{
				messageType = ParseOK;
				return PositioningCommand(messageType, secondPiece, pos, true);
			}
		}
	}

	return PositioningCommand(ParseInvalidCommand);
}

ParserMoveCommand parseMoveCommand(string line) {
	ParserMessageType messageType = ParseInvalidCommand;
	Position pos1 = Position(-1, -1);
	Position pos2 = Position(-1, -1);
	GameMove move = GameMove(pos1, pos2);
	JokerTransform jokerChange = JokerTransform(pos1, (PieceType)-1);

	vector<string> splitted = split(line, ' ');

	if (splitted.size() == 4 || splitted.size() == 8)
	{
		if (atoi(splitted[0].c_str()) != 0 && atoi(splitted[1].c_str()) != 0 && atoi(splitted[2].c_str()) != 0 && atoi(splitted[3].c_str()) != 0)
		{
			Position from = Position(atoi(splitted[0].c_str()), atoi(splitted[1].c_str()));
			Position to = Position(atoi(splitted[2].c_str()), atoi(splitted[3].c_str()));
			if (!from.isInBoard() || !to.isInBoard())
			{
				messageType = ParseOutOfBounds;
				return ParserMoveCommand(messageType, move, jokerChange);
			}
			else
			{
				move = GameMove(from, to);
			}

			if (splitted.size() == 4)
				messageType = ParseOK;

			else if (splitted.size() == 8)
			{
				if (!((splitted[7].length() == 1) || (splitted[7].length() == 2 && (splitted[7][1] == '\n' || splitted[7][1] == '\r'))))
					return ParserMoveCommand(messageType, move, jokerChange);
				else
				{
					PieceType piece = getPieceType(splitted[7][0]);
					if (splitted[4] == "J:" && atoi(splitted[5].c_str()) != 0 && atoi(splitted[6].c_str()) != 0 && (piece == Rock || piece == Paper || piece == Scissors || piece == Bomb))
					{
						Position jokerPos = Position(atoi(splitted[5].c_str()), atoi(splitted[6].c_str()));
						if (jokerPos.isInBoard())
						{
							jokerChange = JokerTransform(jokerPos, piece);
							messageType = ParseOK;
							return ParserMoveCommand(messageType, move, jokerChange);
						}
						else {
							messageType = ParseOutOfBounds;
							return ParserMoveCommand(messageType, move, jokerChange);
						}
							
					}
				}
			}
		}
	}

	return ParserMoveCommand(messageType, move, jokerChange);
}
