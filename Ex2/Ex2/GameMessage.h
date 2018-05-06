#ifndef GameMessage_H
#define GameMessage_H

#include "Fight.h"

// all issues of a move command (relative to the game board)
enum GameMessageType
{
	MoveOK,
	BadMoveFrom,
	BadMoveTo,
	BadMovement,
	NoJokerInPos,
};

class GameMessage
{
private:
	GameMessageType message;
	int playerNum;
	Fight* fightInfo;

public:
	GameMessage(GameMessageType _message, int _playerNum, Fight* _fightInfo = nullptr) : message(_message), playerNum(_playerNum), fightInfo(_fightInfo) {};

	// getters
	GameMessageType getMessage() const { return message; }
	int getPlayerNum() const { return playerNum; }
	Fight* getFightInfo() const { return fightInfo; }

	~GameMessage() { delete(fightInfo); }
};

#endif