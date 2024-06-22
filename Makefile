CC=gcc
CCFLAGS=-Wall -Wextra -Wno-format-truncation -Wno-sign-compare -fPIE
IN=afl.c
OUT=afl
XTR=test

fresh:
	touch $(IN)
all:
	$(CC) -o $(OUT) $(IN) $(CCFLAGS)
run-c:
	./$(XTR)
inter:
	./$(OUT) -i $(XTR).afl
compile:
	./$(OUT) -c $(XTR).afl
caio:
	make all
	make compile
	clear
	./build/$(XTR)
iaio:
	make all
	make inter
