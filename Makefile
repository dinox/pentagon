CC = g++
CPPFLAGS += -I. -fno-tree-vectorize -funroll-loops
HEADERS=benchmark.h interval.h pentagon_dense.h pentagon_fwt.h pentagon.h pentagon_stl.h timer.h pentagon_bp.h macros.h simd_shift_left.h
SOURCES=benchmark

DEBUG ?= 0
AVX ?= 0

ifeq ($(DEBUG),0)
	CPPFLAGS += -O3
else
	CPPFLAGS += -g3 -DDEBUG
endif

ifeq ($(AVX),1)
	CPPFLAGS += -Wa,-q -m64 -march=core2 -msse4.1 -DAVX
endif

all: benchmark

benchmark: benchmark.cpp $(HEADERS)
	$(CC) $(CPPFLAGS) -o benchmark benchmark.cpp

run: clean all
	./benchmark rand 1024 2 1

clean:
	rm -f benchmark
