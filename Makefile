# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -O2
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OUT = aurebesh-droid

all:
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC_FILES) $(shell pkg-config --cflags --libs dpp)