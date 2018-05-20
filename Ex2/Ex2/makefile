COMP = g++-5.3.0
#put all your object files here
OBJS = main.o AutoPlayerAlgorithm.o FilePlayerAlgorithm.o GameBoard.o GameManager.o MainAux.o Minimax.o Parser.o Piece.o Position.o
#The executable filename DON'T CHANGE
EXEC = ex2
CPP_COMP_FLAG = -std=c++14 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) -o $@
#a rule for building a simple c++ source file
#use g++ -MM main.cpp to see dependencies
main.o: main.cpp MainAux.h Piece.h Parser.h GameBoard.h AutoPlayerAlgorithm.h FilePlayerAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
AutoPlayerAlgorithm.o: AutoPlayerAlgorithm.cpp AutoPlayerAlgorithm.h GameBoard.h AlgoPiece.h Fight.h Minimax.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
FilePlayerAlgorithm.o: FilePlayerAlgorithm.cpp FilePlayerAlgorithm.h Parser.h PlayerAlgorithm.h GameBoard.h Fight.h Commands.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
GameBoard.o: GameBoard.cpp GameBoard.h Piece.h GameMessage.h Board.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
GameManager.o: GameManager.cpp GameManager.h FilePlayerAlgorithm.h Fight.h Parser.h GameBoard.h JokerTransform.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
MainAux.o: MainAux.cpp MainAux.h MainAux.h Piece.h GameBoard.h GameManager.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Minimax.o: Minimax.cpp Minimax.h Position.h GameBoard.h Commands.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Parser.o: Parser.cpp Parser.h Piece.h Commands.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Piece.o: Piece.cpp Piece.h Position.h PiecePosition.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Position.o: Position.cpp Position.h Point.h Move.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
clean:
	rm -f $(OBJS) $(EXEC)
