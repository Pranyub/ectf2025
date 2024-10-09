
SOURCE := main.c
OUT := task1
OUTPATH := ./out

CFLAGS := -fno-stack-protector -z execstack -T linker.ld
CC := gcc

all: task1

task1: $(SOURCE)
	mkdir -p $(OUTPATH)
	gcc $(SOURCE) -g -o $(OUTPATH)/$(OUT) $(CFLAGS)

clean:
	rm -rf $(OUTPATH)
