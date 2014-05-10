CC = g++
CPPFLAGS=-I. -fno-tree-vectorize
HEADERS=benchmark.h interval.h pentagon_dense.h pentagon_fwt.h pentagon.h pentagon_stl.h timer.h pentagon_bp.h macros.h
SOURCES=benchmark

DEBUG ?= 0
AVX ?= 0

ifeq ($(DEBUG),0)
	CPPFLAGS += -O3
else
	CPPFLAGS += -g3 -DDEBUG
endif

ifeq ($(AVX),1)
	CPPFLAGS += -Wa,-q -m64 -march=corei7-avx
endif

all: benchmark

benchmark: benchmark.cpp $(HEADERS)
	$(CC) $(CPPFLAGS) -o benchmark benchmark.cpp

run: clean all
	./benchmark rand 256 2 1

clean:
	rm benchmark
