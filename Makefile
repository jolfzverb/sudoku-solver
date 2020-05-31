.PHONY: cmake build test

all: build

cmake:
	mkdir -p build
	pushd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

build: cmake
	pushd build && make all -j5

test: build
	pushd build && make test

clang_format:
	pushd build && make clang_format
