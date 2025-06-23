CXX = g++
CXXFLAGS = -std=c++20 -O2
LDFLAGS = -ldpp -lssl -lcrypto

all: AurebeshDroid

AurebeshDroid: $(wildcard src/*.cpp) aurebesh.hpp
	$(CXX) $(CXXFLAGS) -I/usr/local/include -Iextern/cpp-dotenv/include \
    src/*.cpp -o AurebeshDroid $(LDFLAGS)

run: AurebeshDroid
	./AurebeshDroid

clean:
	rm -f AurebeshDroid