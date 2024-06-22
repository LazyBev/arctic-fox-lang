CC=gcc
CCFLAGS=-Wall -Wextra -Wno-format-truncation -Wno-sign-compare -fPIE
IN=afl.c
OUT=afl
XTR=test

clean:
	rm -rf build
	mkdir build
cc:
	$(CC) -o $(OUT) $(IN) $(CCFLAGS)
compile:
	./$(OUT) -c $(XTR).afl
run:
	./$(OUT) -r $(XTR).afl
all:
	make clean
	make cc
	clear
	make run
