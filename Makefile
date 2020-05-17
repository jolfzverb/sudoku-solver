.PHONY: test clang-format

solver: solver.cpp
	g++ -std=c++2a -Wall -pedantic solver.cpp -o solver

test:
	./solver < test

clang-format:
	clang-format -i solver.cpp
