# Makefile

$(info ✅ MAKEFILE RAN ✅)

CXX = g++
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OUT = aurebesh-droid

CXXFLAGS ?= -std=c++17 -O2 -I/opt/homebrew/opt/openssl/include
LDFLAGS ?= -L/opt/homebrew/opt/openssl/lib -ldpp -lpthread -lssl -lcrypto -lcurl

all:
	echo "🛠️ Building: $(SRC_FILES)"
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC_FILES) $(LDFLAGS)

clean:
	rm -f $(OUT)