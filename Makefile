# Makefile

$(info ✅ MAKEFILE WAS EXECUTED)

CXX = g++
CXXFLAGS = -std=c++17 -O2
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OUT = aurebesh-droid

DPP_FLAGS = $(shell pkg-config --cflags --libs dpp 2>/dev/null || echo "-ldpp -lpthread -lssl -lcrypto -lcurl")

all:
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC_FILES) $(DPP_FLAGS)

clean:
	rm -f $(OUT)