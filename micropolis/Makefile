CXX  := g++

.PHONY: clean 3rdparty

dapp: dapp.cpp $(wildcard src/*.cpp)
	make -C 3rdparty
	$(CXX) -pthread -std=c++20 -o $@ $^

clean:
	@rm -rf dapp
	make -C 3rdparty clean
