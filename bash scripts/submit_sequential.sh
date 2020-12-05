#!/bin/bash
#SBATCH --time=00:20:00
#SBATCH --partition=batch
#SBATCH --cpus-per-task=20  
cd final

make clean

module rm gcc
module rm OpenCilk/9.0.1

module load gcc
make all

module load OpenCilk/9.0.1
make cilk

echo "v3 results" > ./results/v3.txt
	
echo "Number of workers: 2 4 5 10 15 20 40" >> ./results/v3.txt
for file in ./matrices/*; do
	printf "\n"
	echo "Running v3 for matrix: "$file >> ./results/v3.txt
	for i in 2 4 5 10 15 20 40; do
		./v3 $file $i 1 >> ./results/v3.txt
	done
	echo "" >> ./results/v3.txt
	echo "" >> ./results/v3.txt
done


echo "v4 results" > ./results/v4.txt
	
echo "Number of workers: 2 4 5 10 15 20 40" >> ./results/v4.txt
for file in ./matrices/*; do
	printf "\n"
	echo "Running v4 for matrix: "$file >> ./results/v4.txt
	for i in 2 4 5 10 15 20 40; do
		./v4 $file $i 1 >> ./results/v4.txt
	done
	echo "" >> ./results/v4.txt
	echo "" >> ./results/v4.txt
done


cd ..
