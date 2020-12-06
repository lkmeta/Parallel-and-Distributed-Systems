#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH --partition=batch

rm -r results

make clean

module rm gcc
module rm OpenCilk/9.0.1

module load gcc
make all

module load OpenCilk/9.0.1
make cilk

mkdir results

echo "v3_cilk results" > ./results/v3_cilk.txt
	
echo "Number of workers: 2 4 5 10 15 20" >> ./results/v3_cilk.txt
echo "" >> ./results/v3_cilk.txt
for file in ./matrices/*; do
	printf "\n"
	echo "Running v3_cilk for matrix: "$file >> ./results/v3_cilk.txt
	for i in 2 4 5 10 15 20; do
		CILK_NWORKERS=$i
		./v3_cilk $file $i 1 >> ./results/v3_cilk.txt	
	done
	echo "" >> ./results/v3_cilk.txt
	echo "" >> ./results/v3_cilk.txt
done



echo "v3_openMP results" > ./results/v3_openMP.txt
	
echo "Number of threads: 2 4 5 10 15 20" >> ./results/v3_openMP.txt
for file in ./matrices/*; do
	printf "\n"
	echo "Running v3_openMP for matrix: "$file >> ./results/v3_openMP.txt
	for i in 2 4 5 10 15 20; do
		./v3_openMP $file $i 1 >> ./results/v3_openMP.txt
	done
	echo "" >> ./results/v3_openMP.txt
	echo "" >> ./results/v3_openMP.txt
done


echo "v4_cilk results" > ./results/v4_cilk.txt
	
echo "Number of workers: 2 4 5 10 15 20" >> ./results/v4_cilk.txt
for file in ./matrices/*; do
	printf "\n"
	echo "Running v4_cilk for matrix: "$file >> ./results/v4_cilk.txt
	for i in 2 4 5 10 15 20; do
		CILK_NWORKERS=$i
		./v4_cilk $file $i 1 >> ./results/v4_cilk.txt
	done
	echo "" >> ./results/v4_cilk.txt
	echo "" >> ./results/v4_cilk.txt
done



echo "v4_openMP results" > ./results/v4_openMP.txt
	
echo "Number of threads: 2 4 5 10 15 20" >> ./results/v4_openMP.txt
for file in ./matrices/*; do
	printf "\n"
	echo "Running v4_openMP for matrix: "$file >> ./results/v4_openMP.txt
	for i in 2 4 5 10 15 20; do
		./v4_openMP $file $i 1 >> ./results/v4_openMP.txt
	done
	echo "" >> ./results/v4_openMP.txt
	echo "" >> ./results/v4_openMP.txt
done
