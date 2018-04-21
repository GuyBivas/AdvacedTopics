#include "Parser.h"

using namespace std;

PieceType getPieceType(char c)
{
	switch (c)
	{
	case 'R':
		return Rock;
	case 'P':
		return Paper;
	case 'S':
		return Scissors;
	case 'B':
		return Bomb;
	case 'F':
		return Flag;
	case 'J':
		return Joker;
	default:
		return (PieceType)-1;
	}
}

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

PostioningCommand::PostioningCommand(string line)
{
	messageType = ParseInvalidCommand;
	vector<string> splitted = split(line, ' ');
	if (splitted.size() == 0 || splitted[0].length() != 1)
		return;

	PieceType piece = getPieceType(splitted[0][0]);
	if (piece == -1)
		return;

	else if (splitted.size() == 3) // normal piece
	{
		if (atoi(splitted[1].c_str()) != 0 && atoi(splitted[2].c_str()) != 0)
		{
			Position _pos = Position(atoi(splitted[1].c_str()), atoi(splitted[2].c_str()));
			if (!_pos.isInBoard())
			{
				messageType = ParseOutOfBounds;
				return;
			}
			else if (piece == Joker)
				return;
			else 
			{
				messageType = ParseOK;
				pos = _pos;
				type = piece;
				isJoker = false;
				return;
			}
		}
	}
	else if (splitted.size() == 4) // joker piece
	{
		if (!((splitted[3].length() == 1) || (splitted[3].length() == 2 && (splitted[3][1] == '\n' || splitted[3][1] == '\r'))))
			return;

		PieceType secondPiece = getPieceType(splitted[3][0]);
		if (piece == Joker && atoi(splitted[1].c_str()) != 0 && atoi(splitted[2].c_str()) != 0 && secondPiece != -1)
		{
			Position _pos = Position(atoi(splitted[1].c_str()), atoi(splitted[2].c_str()));
			if (!_pos.isInBoard())
			{
				messageType = ParseOutOfBounds;
				return;
			}
			else if (secondPiece == Joker || secondPiece == Flag)
			{
				messageType = ParseInvalidJokerRep;
			}
			else
			{
				messageType = ParseOK;
				pos = _pos;
				type = secondPiece;
				isJoker = true;
				return;
			}
		}
	}
}

MoveCommand::MoveCommand(string line)
{
	messageType = ParseInvalidCommand;

	vector<string> splitted = split(line, ' ');

	if (splitted.size() == 4 || splitted.size() == 8)
	{
		if (atoi(splitted[0].c_str()) != 0 && atoi(splitted[1].c_str()) != 0 && atoi(splitted[2].c_str()) != 0 && atoi(splitted[3].c_str()) != 0)
		{
			Position _from = Position(atoi(splitted[0].c_str()), atoi(splitted[1].c_str()));
			Position _to = Position(atoi(splitted[2].c_str()), atoi(splitted[3].c_str()));
			if (!_from.isInBoard() || !_to.isInBoard())
			{
				messageType = ParseOutOfBounds;
				return;
			}
			else
			{
				from = _from;
				to = _to;
			}

			if (splitted.size() == 4)
				messageType = ParseOK;

			else if (splitted.size() == 8)
			{
				if (!((splitted[7].length() == 1) || (splitted[7].length() == 2 && (splitted[7][1] == '\n' || splitted[7][1] == '\r'))))
					return;
				else
				{
					PieceType piece = getPieceType(splitted[7][0]);
					if (splitted[4] == "J:" && atoi(splitted[5].c_str()) != 0 && atoi(splitted[6].c_str()) != 0 && (piece == Rock || piece == Paper || piece == Scissors || piece == Bomb))
					{
						Position _jokerPos = Position(atoi(splitted[5].c_str()), atoi(splitted[6].c_str()));
						if (_jokerPos.isInBoard())
						{
							jokerPos = _jokerPos;
							jokerNewRep = piece;
							messageType = ParseOK;
						}
						else
							messageType = ParseOutOfBounds;
					}
				}
			}
		}
	}
}