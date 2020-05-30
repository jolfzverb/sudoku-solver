.PHONY: test clang-format

solver: solver.cpp state.cpp box_iterator.cpp state.hpp box_iterator.hpp
	g++ -g -std=c++2a -Wall -pedantic solver.cpp state.cpp box_iterator.cpp -o solver

test: solver
	./solver < test

clang-format:
	clang-format -i *.cpp *.hpp
