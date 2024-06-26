CC=gcc
CCFLAGS=-Wall -Wextra -Wno-format-truncation -Wno-sign-compare -fPIE
IN=broke.c
OUT=afl
XTR=test

clean:
	rm test.s
	rm test.o
	rm test
build:
	$(CC) -o $(OUT) $(IN)
	./$(OUT) -c $(XTR).afl $(XTR).s
	nasm -felf64 -o $(XTR).o $(XTR).s
	ld -o $(XTR) $(XTR).o
run:
	./$(XTR)
compile:
	make clean
	make build
