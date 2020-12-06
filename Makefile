CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3 

default: all

v1:
	$(CC) $(CFLAGS) -o v1 v1.c

v2:
	$(CC) $(CFLAGS) -o v2 v2.c

v3:
	$(CC) $(CFLAGS) -o v3 v3.c mmio.c

v3_cilk:
	$(CILKCC) $(CFLAGS) -o v3_cilk v3_cilk.c mmio.c -fcilkplus

v3_openMP: 
	$(CC) $(CFLAGS) -o v3_openMP v3_openMP.c mmio.c -fopenmp

v4:
	$(CC) $(CFLAGS) -o v4 v4.c mmio.c

v4_cilk:
	$(CILKCC) $(CFLAGS) -o v4_cilk v4_cilk.c mmio.c -fcilkplus

v4_openMP: 
	$(CC) $(CFLAGS) -o v4_openMP v4_openMP.c mmio.c -fopenmp

.PHONY: clean

all: v1 v2 v3 v3_cilk v3_openMP v4 v4_cilk v4_openMP

test_v3: test1 test2 test3 test4 test5

test_v4: test6 test7 test8 test9 test10

test:
	@printf "\n** Testing v1 \n"
	./v1
	@printf "\n** Testing v2 \n"
	./v2
test1:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v3 with mycielskian13.mtx\n"
	./v3 ./matrices/mycielskian13.mtx
	@printf "\n** Testing v3_cilk with mycielskian13.mtx with 4 workers\n"
	./v3_cilk ./matrices/mycielskian13.mtx 4
	@printf "\n** Testing v3_openMp with mycielskian13.mtx with 4 threads\n"
	./v3_openMP ./matrices/mycielskian13.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test2:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v3 with dblp-2010.mtx\n"
	./v3 ./matrices/dblp-2010.mtx
	@printf "\n** Testing v3_cilk with dblp-2010.mtx with 4 workers\n"
	./v3_cilk ./matrices/dblp-2010.mtx 4
	@printf "\n** Testing v3_openMp with dblp-2010.mtx with 4 threads\n"
	./v3_openMP ./matrices/dblp-2010.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test3:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v3 with com-Youtube.mtx\n"
	./v3 ./matrices/belgium_osm.mtx
	@printf "\n** Testing v3_cilk with com-Youtube.mtx with 4 workers\n"
	./v3_cilk ./matrices/belgium_osm.mtx 4
	@printf "\n** Testing v3_openMp with com-Youtube.mtx with 4 threads\n"
	./v3_openMP ./matrices/belgium_osm.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test4:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v3 with com-Youtube.mtx\n"
	./v3 ./matrices/com-Youtube.mtx
	@printf "\n** Testing v3_cilk with com-Youtube.mtx with 4 workers\n"
	./v3_cilk ./matrices/com-Youtube.mtx 4
	@printf "\n** Testing v3_openMp with com-Youtube.mtx with 4 threads\n"
	./v3_openMP ./matrices/com-Youtube.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test5:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v3 with NACA0015.mtx\n"
	./v3 ./matrices/NACA0015.mtx
	@printf "\n** Testing v3_cilk with NACA0015.mtx with 4 workers\n"
	./v3_cilk ./matrices/NACA0015.mtx 4
	@printf "\n** Testing v3_openMp with NACA0015.mtx with 4 threads\n"
	./v3_openMP ./matrices/NACA0015.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"


test6:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v4 with mycielskian13.mtx\n"
	./v4 ./matrices/mycielskian13.mtx
	@printf "\n** Testing v4_cilk with mycielskian13.mtx with 4 workers\n"
	./v4_cilk ./matrices/mycielskian13.mtx 4
	@printf "\n** Testing v4_openMp with mycielskian13.mtx with 4 threads\n"
	./v4_openMP ./matrices/mycielskian13.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test7:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v4 with dblp-2010.mtx\n"
	./v4 ./matrices/dblp-2010.mtx
	@printf "\n** Testing v4_cilk with dblp-2010.mtx with 4 workers\n"
	./v4_cilk ./matrices/dblp-2010.mtx 4
	@printf "\n** Testing v4_openMp with dblp-2010.mtx with 4 threads\n"
	./v4_openMP ./matrices/dblp-2010.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test8:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v4 with belgium_osm.mtx\n"
	./v4 ./matrices/belgium_osm.mtx
	@printf "\n** Testing v4_cilk with belgium_osm.mtx with 4 workers\n"
	./v4_cilk ./matrices/belgium_osm.mtx 4
	@printf "\n** Testing v4_openMp with belgium_osm.mtx with 4 threads\n"
	./v4_openMp ./matrices/belgium_osm.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test9:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v4 with com-Youtube.mtx\n"
	./v4 ./matrices/com-Youtube.mtx
	@printf "\n** Testing v4_cilk with com-Youtube.mtx with 4 workers\n"
	./v4_cilk ./matrices/com-Youtube.mtx 4
	@printf "\n** Testing v4_openMp with com-Youtube.mtx with 4 threads\n"
	./v4_openMP ./matrices/com-Youtube.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

test10:
	@printf "\n==================================================\n"
	@printf "==================================================\n"
	@printf "\n** Testing v4 with NACA0015.mtx\n"
	./v4 ./matrices/NACA0015.mtx
	@printf "\n** Testing v4_cilk with NACA0015.mtx with 4 workers\n"
	./v4_cilk ./matrices/NACA0015.mtx 4
	@printf "\n** Testing v4_openMp with NACA0015.mtx with 4 threads\n"
	./v4_openMP ./matrices/NACA0015.mtx 4
	@printf "\n==================================================\n"
	@printf "==================================================\n"

clean:
	rm v1 v2 v3 v3_cilk v3_openMP v4 v4_cilk v4_openMP
