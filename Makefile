ifeq ($(origin CC), default)
CC = gcc
endif
CFLAGS ?= -c -std=c99 -g #-Wpedantic -ffunction-sections -fdata-sections
LFLAGS ?=  #-ffunction-sections -fdata-sections
OFLAGS ?= -o

all: _build build/asc
_build: | build

build/asc: build/main.o build/lex.o build/machine.o
	$(CC) $(LFLAGS) $(OFLAGS) $@ $^

build:
	mkdir -p build

build/main.o: src/main.c
	$(CC) $(CFLAGS) $(OFLAGS) $@ $<

build/lex.o: src/lex.c
	$(CC) $(CFLAGS) $(OFLAGS) $@ $<

build/machine.o: src/machine.c
	$(CC) $(CFLAGS) $(OFLAGS) $@ $<


test: all FORCE
	build/asc -file=test/test.asc

install: all
	cp build/asc /usr/bin/asc

FORCE:
