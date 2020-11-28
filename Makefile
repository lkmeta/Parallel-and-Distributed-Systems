CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

v3:
	$(CC) $(CFLAGS) -o v3 v3_version2.c mmio.c

v3_cilk:
	$(CILKCC) $(CFLAGS) -o v3_cilk v3_cilk.c mmio.c -fcilkplus

v3_openMP: 
	$(CC) $(CFLAGS) -o v3_openMP v3_openMP.c mmio.c -fopenmp

.PHONY: clean

all: v3 v3_cilk v3_openMP

test1:
	@printf "\n** Testing v3 with no1.mtx\n"
	./v3 no1.mtx
	@printf "\n** Testing v3_cilk with no1.mtx\n"
	./v3_cilk no1.mtx
	@printf "\n** Testing v3_cilk with no1.mtx\n"
	./v3_openMP no1.mtx

test2:
	@printf "\n** Testing v3 with com-Youtube.mtx\n"
	./v3 com-Youtube.mtx
	@printf "\n** Testing v3_cilk with com-Youtube.mtx\n"
	./v3_cilk com-Youtube.mtx
	@printf "\n** Testing v3_cilk with com-Youtube.mtx\n"
	./v3_openMP com-Youtube.mtx
	

clean:
	rm v3 v3_cilk v3_openMP
