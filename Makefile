CC = g++
override CPPFLAGS+=-I. -fno-tree-vectorize -funroll-loops
HEADERS=benchmark.h interval.h pentagon_dense.h pentagon_fwt.h pentagon.h pentagon_stl.h timer.h pentagon_bp.h macros.h simd_shift_left.h pentagon_simd.h
SOURCES=benchmark

DEBUG ?= 0
AVX ?= 0
SSE ?= 0
IVY ?= 0

ifeq ($(DEBUG),0)
	override CPPFLAGS += -O3
else
	override CPPFLAGS += -g3 -DDEBUG
endif

ifeq ($(AVX),1)
	override CPPFLAGS += -DAVX
endif

ifeq ($(IVY), 1)
	override CPPFLAGS += -Wa,-q -m64 -march=corei7-avx
endif

ifeq ($(SSE),1)
	override CPPFLAGS += -DSSE
endif

ifeq ($(CORE2),1)
	override CPPFLAGS += -m64 -march=core2 -msse4.1 -DSSE
endif

all: benchmark

benchmark: benchmark.cpp $(HEADERS)
	$(CC) $(CPPFLAGS) -c -o benchmark.o benchmark.cpp
	$(CC) $(CPPFLAGS) -o benchmark benchmark.o

disas: benchmark
	otool -vt benchmark.o

run: clean all
	./benchmark benchfile b4096

clean:
	rm -f benchmark
