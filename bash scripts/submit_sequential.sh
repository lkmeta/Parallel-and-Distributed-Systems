#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH --partition=batch
#SBATCH --cpus-per-task=2 

cd Desktop

make clean

module rm gcc
module rm OpenCilk/9.0.1

module load gcc
make v3 
make v4

mkdir results

echo "v3 results" > ./results/v3.txt
	
for file in ./matrices/*; do
	printf "\n"
	echo "Running v3 for matrix: "$file >> ./results/v3.txt
	./v3 $file >> ./results/v3.txt
	echo "" >> ./results/v3.txt
done

echo "v4 results" > ./results/v4.txt
	
for file in ./matrices/*; do
	printf "\n"
	echo "Running v4 for matrix: "$file >> ./results/v4.txt
    ./v4 $file >> ./results/v4.txt
	echo "" >> ./results/v4.txt
done
