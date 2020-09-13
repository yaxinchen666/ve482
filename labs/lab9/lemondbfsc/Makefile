CC=gcc
CXX=g++
CFLAGS=-Wall -c -D_FILE_OFFSET_BITS=64 -lm
CXXFLAGS=-std=c++14 -Wall -Wextra -Werror -pedantic -Wno-unused-result -Wno-unused-parameter 
CPP=$(wildcard *.cpp ./query/*.cpp ./query/data/*.cpp ./query/management/*.cpp ./db/*.cpp)
O_FILES=$(patsubst %.cpp,%.o,$(CPP))

all: build
 
build: $(O_FILES) lemondbfs.o
	$(CXX) $(CXXFLAGS) $(O_FILES) lemondbfs.o -o lemondbfs `pkg-config fuse --cflags --libs`

lemondbfs.o: lemondbfs.c
	$(CC) $(CFLAGS) lemondbfs.c -o `pkg-config fuse --cflags --libs`

clean:
	find . -type f -name '*.o' -exec rm {} +
