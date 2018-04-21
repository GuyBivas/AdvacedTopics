#ifndef FIGHT_INFO
#define FIGHT_INFO

class FightInfo {

public:
	virtual const Point& getPosition() const = 0;
	virtual char getOpponentPiece() const = 0; // R, P, S, B or F (but NOT J)
	virtual int getWinner() const = 0; // 0 - both lost, 1 - player 1 won, 2 - player 2 won
};

#endif // !FIGHT_INFO
