#ifndef ALGO_PIECE
#define ALGO_PIECE

#include "Piece.h"

class AlgoPiece : public Piece {
private:
	bool isKnown;
	bool hasMoved = false; // -1 if moved at some point of the game

public:
	/*Piece(PieceType _type, Point& _pos, int _playerNum, bool _isJoker = false) :
		type(_type), pos(_pos), playerNum(_playerNum), isJoker(_isJoker) {};*/

	AlgoPiece(PieceType _type, Point& _pos, int _playerNum, bool _isJoker, bool _isKnown) : Piece(_type, _pos, _playerNum, _isJoker), isKnown(_isKnown)	{ }

	// getters
	bool getIsKnown() const { return isKnown; }
	bool getHasMoved() const { return hasMoved; }
	
	// setters
	void setIsKnown(bool _isKnown) { isKnown = _isKnown; }
	void setHasMoved() { hasMoved = true; }
};

#endif