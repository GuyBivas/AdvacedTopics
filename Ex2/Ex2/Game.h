#ifndef Game_H
#define Game_H

#include <string>
#include "Piece.h"
#include "GameMessage.h"

using namespace std;

// end game reasons (and normal status)
enum GameStatus
{
	StatusNormal,
	StatusPlayer1FlagsCaptured,
	StatusPlayer2FlagsCaptured,
	StatusBothFlagsCaptured,
	StatusPlayer1NoMovingPieces,
	StatusPlayer2NoMovingPieces,
	StatusBothPlayersNoMovingPieces
};

// hold the game board and other game info and manage all game functionalities
class Game
{
private:
	Piece* board[ROWS][COLS];
	int currentPlayer = 1;
public:
	Game();
	~Game();

	// wrap getPieceAt(pos) with [] operator
	Piece* operator[](Position pos)
	{
		return getPieceAt(pos);
	}

	// get representation of the board
	string getBoardRep();

	// free board[to] and set board[to] = board[from]
	void movePiece(Position const& from, Position const& to);

	// checks if a move is valid in this board state
	GameMessage isValidMove(Position const& from, Position const& to);

	// treat all move aspects (validation, movePiece, fight)
	GameMessage move(Position const &from, Position const& to);

	// transforms the joker as part of a move
	GameMessage transformJoker(Position const& jokerPos, PieceType jokerNewType = (PieceType)-1);

	// return enum indicating whether game is over and by what reason
	GameStatus getGameStatus();

	// treat joker representation change
	GameMessage changeJoker(Position jokerPos, PieceType newRep);

	// set piece in a position on the board
	void setPos(Position pos, Piece* piece) { board[pos.getRow() - 1][pos.getCol() - 1] = piece; }

	// check whether a position on the board contains piece of current player
	bool containsCurrPlayerPiece(Position const pos)
	{
		return isInBoard(pos) && getPieceAt(pos) != NULL && getPieceAt(pos)->getPlayerNum() == currentPlayer;
	}

	// sets the current player. It is assumed function input is either 1 or 2.
	void setCurrPlayer(int player) { currentPlayer = player; }

	// check whether a position is within the board
	bool isInBoard(Position const& pos)
	{
		return pos.isInBoard();
		//return pos.getRow() <= ROWS && pos.getCol() <= COLS && pos.getRow() > 0 && pos.getCol() > 0;
	};

	// return board[pos]
	Piece* getPieceAt(Position pos)
	{
		if (!isInBoard(pos))
			return NULL;

		return board[pos.getRow()-1][pos.getCol()-1];
	}

	// get char representation of piece or empty representation if piece is null
	char getPosRep(Position pos)
	{
		Piece* piece = getPieceAt(pos);
		if (piece == NULL)
			return ' ';

		return piece->getRep();
	}

	int getCurrentPlayer() const { return currentPlayer; }

	// gets the other player (of the current player)
	int getOtherPlayer() const;
};

#endif