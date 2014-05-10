CC = g++
CPPFLAGS=-I. -fno-tree-vectorize
HEADERS=benchmark.h interval.h pentagon_dense.h pentagon_fwt.h pentagon.h pentagon_stl.h timer.h pentagon_bp.h macros.h
SOURCES=benchmark

DEBUG ?= 0

ifeq ($(DEBUG),0)
	CPPFLAGS += -O2
else
	CPPFLAGS += -g3 -DDEBUG
endif

all: benchmark

benchmark: benchmark.cpp $(HEADERS)
	$(CC) $(CPPFLAGS) -o benchmark benchmark.cpp

run: all
	./benchmark rand 256 2 1

erik:
	c++ -o test.o -c test.cpp
	c++ -o test test.o
	./test

clean:
	rm benchmark
