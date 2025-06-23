# Makefile

all:
	@cmake -Bbuild -S . -DCMAKE_BUILD_TYPE=Release
	@cmake --build build --parallel
	@cp build/AurebeshDroid .

clean:
	rm -rf build AurebeshDroid