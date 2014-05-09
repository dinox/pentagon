CC = g++
CPPFLAGS=-I. -fno-tree-vectorize
HEADERS=interval.h pentagon_dense.h pentagon.h pentagon_stl.h timer.h
SOURCES=benchmark.cpp
#test_impl.cpp test_timer.cpp
DEBUG ?= 0

ifeq ($(DEBUG),0)
	CPPFLAGS += -O2
else
	CPPFLAGS += -g3 -DDEBUG
endif

all: $(SOURCES:.cpp=.bin)

%.bin: %.cpp $(HEADERS)
	$(CC) -o $@ $< $(CPPFLAGS)

clean:
	rm -r *.bin
