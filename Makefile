CC=gcc
CFLAGS=-g -Wall
LFLAGS=
EXE=server client

all: $(EXE)

clean:
	rm -rf $(EXE)
