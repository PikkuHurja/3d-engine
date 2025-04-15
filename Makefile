CXX = c++
CXXFLAGS = -std=c++20 -Wall -O2 -lSDL3 -lOpenGL -lGLEW -I/home/pikku/code/sdl3/head/ -I/home/pikku/code/engine/ -I$(LIB.D)
NAME = program
NAMEFILE = $(BIN.D)$(NAME)

SRC.D = src/
BIN.D = bin/
LIB.D = lib/

# Find all .cpp files in src/
SRCS := $(wildcard $(SRC.D)/*.cpp)
# Generate corresponding .o files in bin/
OBJS := $(patsubst $(SRC.D)/%.cpp,$(BIN.D)/%.o,$(SRCS))


all: $(NAMEFILE) exec



# Compile each .cpp file to .o file
$(BIN.D)/%.o: $(SRC.D)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAMEFILE): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $(NAMEFILE)
	rm $^
	

.PHONY: exec
exec:
	@echo "Starting Execution:"
	@ ./$(NAMEFILE)

