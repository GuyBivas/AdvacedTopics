#ifndef GAME_BOARD
#define GAME_BOARD

#include <string>
#include <functional>  

#include "Piece.h"
#include "AlgoPiece.h"
#include "GameMessage.h"
#include "Board.h"

using namespace std;

// maximum number of piece types to position on the board
#define ROCK_COUNT 2
#define PAPER_COUNT 5
#define SCISSORS_COUNT 1
#define BOMB_COUNT 2
#define JOKER_COUNT 2
#define FLAG_COUNT 1

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
class GameBoard : public Board {
private:
	Piece* board[ROWS][COLS];
	int currentPlayer = 1;

public:
	GameBoard();
	GameBoard(const GameBoard& boardToCopy);
	~GameBoard() override;

	virtual int getPlayer(const Point& pos) const;

	// wrap getPieceAt(pos) with [] operator
	Piece* operator[](const Point& pos)
	{
		return getPieceAt(pos);
	}

	// get representation of the board
	string getBoardRep();

	// free board[to] and set board[to] = board[from]
	void movePiece(const GameMove& move);

	// checks if a move is valid in this board state
	GameMessage isValidMove(const GameMove& move);

	// treat all move aspects (validation, movePiece, fight)
	GameMessage move(const GameMove& move);

	// transforms the joker as part of a move
	GameMessage transformJoker(const Point& jokerPos, PieceType jokerNewType = (PieceType)-1);

	// return enum indicating whether game is over and by what reason
	GameStatus getGameStatus() const;

	// treat joker representation change
	GameMessage changeJoker(Point& jokerPos, PieceType newRep);

	// set piece in a position on the board
	void setPos(const Point& pos, Piece* piece, bool deletePos = true)
	{
		if (deletePos)
			delete(board[pos.getY() - 1][pos.getX() - 1]);

		board[pos.getY() - 1][pos.getX() - 1] = piece;
	}

	// Put a piece in its position on the board
	void putPiece(Piece* piece)
	{
		Position pos = piece->getPosition();
		delete(board[pos.getY() - 1][pos.getX() - 1]);
		board[pos.getY() - 1][pos.getX() - 1] = piece;
	}

	// check whether a position on the board contains piece of current player
	bool containsCurrPlayerPiece(const Point& pos)
	{
		return isInBoard(pos) && getPieceAt(pos) != nullptr && getPieceAt(pos)->getPlayerNum() == currentPlayer;
	}

	// sets the current player. It is assumed function input is either 1 or 2.
	void setCurrPlayer(int player) { currentPlayer = player; }

	// check whether a position is within the board
	static bool isInBoard(const Point& pos)
	{
		return pos.getY() <= ROWS && pos.getX() <= COLS && pos.getY() > 0 && pos.getX() > 0;
	};

	// return board[pos]
	Piece* getPieceAt(const Point& pos) const
	{
		//if (!isInBoard(pos))
		//	return nullptr;

		return board[pos.getY()-1][pos.getX()-1];
	}

	// return board[pos]
	Piece* getPieceAt(int x, int y) const
	{
		return board[y - 1][x - 1];
	}

	// get char representation of piece or empty representation if piece is null
	char getPosRep(Point& pos)
	{
		Piece* piece = getPieceAt(pos);
		if (piece == nullptr)
			return ' ';

		return piece->getPiece();
	}

	bool isGameOver() const;

	int getCurrentPlayer() const { return currentPlayer; }

	// gets the other player (of the current player)
	int getOtherPlayer() const;

	int countPiecesScore(function<int(Piece*)> countFunc = ([](Piece* p) { return 1; }));
	int countPiecesIf(function<bool(Piece*)> conditionFunc = nullptr);
};

int getOppositePlayer(int player);


#endif