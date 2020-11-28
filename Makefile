CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

v3:
	$(CC) $(CFLAGS) -o v3 v3_version2.c mmio.c

v3_cilk:
	$(CILKCC) $(CFLAGS) -o v3_cilk v3_cilk.c mmio.c -fcilkplus

.PHONY: clean

all: v3 v3_cilk

test:
	@printf "\n** Testing v3 with no1.mtx\n\n"
	./v3 no1.mtx
	@printf "\n** Testing v3_cilk with no1.mtx\n\n"
	./v3_cilk no1.mtx


clean:
	rm v3 v3_cilk