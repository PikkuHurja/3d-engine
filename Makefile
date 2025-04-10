CXX = ccache c++
CXXFLAGS = -Wall -O2 -I$(LIB.D)
NAME = program

SRC.D = src/
BIN.D = bin/
LIB.D = lib/


$(BIN.D)main.o: $(SRC.D)main.cpp 
	$(CXX) $(CXXFLAGS) -c $^ -o $@

compile: $(BIN.D)main.o
	$(CXX) $(CXXFLAGS) $^ -o bin/$(NAME)

