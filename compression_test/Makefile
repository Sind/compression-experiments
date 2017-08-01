
CC=gcc

CXX = g++
CFLAGS= -std=c99 -ggdb
CXXFLAGS=-ggdb -Wall -Wextra
OBJ = src/main.o src/LZW/lzw.o src/LZW_MC/lzw_mc.o src/LZ4/lz4.o src/fpaq0/fpaq0-c.o src/PPMs/Model.o src/PPMs/PPMs.o src/fpaq0/fpaq0.o src/snappy/snappy.o src/snappy/snappy-c.o src/snappy/snappy-sinksource.o src/snappy/snappy-stubs-internal.o

main: $(OBJ)
	$(CXX) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $<

clean:
	rm $(OBJ)
	rm ./main
