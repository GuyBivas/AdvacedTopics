#ifndef GameMessage_H
#define GameMessage_H

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
public:
	GameMessage(GameMessageType _message, int _playerNum) : message(_message), playerNum(_playerNum) {};

	// getters
	GameMessageType getMessage() const { return message; }
	int getPlayerNum() const { return playerNum; }
};

#endif