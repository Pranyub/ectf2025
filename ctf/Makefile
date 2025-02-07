CC=gcc
CXX=g++
CFLAGS=
DFLAGS=-g -Og
BFLAGS=-O2
LFLAGS=
LIBS=

debug: override CFLAGS+=$(DFLAGS)
debug: all

build: override CFLAGS+=$(BFLAGS)
build: all

all: decrypt_test

decrypt_test: decrypt_test.o hash.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

decrypt.o: decrypt.c decrypt.h common.h hash.h
	$(CC) $(CFLAGS) -c -o $@ $<

hash.o: hash.c hash.h common.h
	$(CC) $(CFLAGS) -c -o $@ $<

decrypt_test.o: decrypt_test.c decrypt.c decrypt.h common.h hash.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o decrypt_test
