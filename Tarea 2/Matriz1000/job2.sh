#!/bin/bash

#SBATCH --partition=full

#SBATCH --job-name=IMT2112
#SBATCH --output=log2.out

#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1

mpic++ tarea2_read_matrix.cpp
mpirun a.out