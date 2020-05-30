.PHONY: test clang-format

solver: main.cpp state.cpp box_iterator.cpp state.hpp box_iterator.hpp solver.cpp solver.hpp
	g++ -g -std=c++2a -Wall -pedantic main.cpp state.cpp box_iterator.cpp solver.cpp -o solver

test: solver
	./solver < test

clang-format:
	clang-format -i *.cpp *.hpp
