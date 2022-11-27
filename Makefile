#!/bin/make

.PHONY: clean run valgrind

CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O1 -pthread

BIN=bin/chase
TEST_DIR=./test/
SRC:=$(wildcard src/*.c)
INCLUDES:=$(wildcard src/*.h)
OBJ:=$(addprefix obj/, main.o directory.o thread.o)

VAL_FLAGS=--leak-check=full --trace-children=yes

all: $(BIN)

$(BIN): $(OBJ) bin/
	$(CC) $(CFLAGS) -o $@ $(OBJ)

obj/%.o: src/%.c $(INCLUDES) obj/
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(BIN)
	./$(BIN) $(TEST_DIR)

valgrind: $(BIN)
	valgrind $(VAL_FLAGS) $(BIN) $(TEST_DIR)

obj/:
	mkdir obj/
bin/:
	mkdir bin/
clean:
	rm -rf bin/* obj/* $(BIN)
