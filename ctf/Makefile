CC=gcc
CXX=g++
CFLAGS=
DFLAGS=-g -Og
BFLAGS=-O2
LFLAGS=
LIBS=libwolfssl.a

debug: override CFLAGS+=$(DFLAGS)
debug: all

build: override CFLAGS+=$(BFLAGS)
build: all

all: testmain

testmain: testmain.o decrypt.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

decrypt.o: decrypt.c decrypt.h common.h
	$(CC) $(CFLAGS) -I . -c -o $@ $<

testmain.o: testmain.c common.h decrypt.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o testmain
