# Makefile

CXX       := g++
CXXFLAGS  := -std=c++20 -O2 -Wall -Wextra -pedantic \
             -I./include
LDFLAGS   := -lpthread -lssl -lcrypto

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)
BIN := aurebesh-droid

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)