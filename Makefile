CC = g++
CPPFLAGS=-I. -fno-tree-vectorize
HEADERS=interval.h pentagon_dense.h pentagon.h pentagon_stl.h timer.h
SOURCES=benchmark
#test_impl.cpp test_timer.cpp
DEBUG ?= 0

ifeq ($(DEBUG),0)
	CPPFLAGS += -O2
else
	CPPFLAGS += -g3 -DDEBUG
endif

all: benchmark run

benchmark:
	$(CC) -o benchmark -c benchmark.cpp

run:
	./benchmark

erik:
	c++ -o test.o -c test.cpp
	c++ -o test test.o
	./test

clean:
	rm -r *.bin
