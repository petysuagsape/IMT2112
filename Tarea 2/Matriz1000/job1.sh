#!/bin/bash

#SBATCH --partition=full

#SBATCH --job-name=IMT2112
#SBATCH --output=log.out

#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1

mpic++ tarea2_read_matrix.cpp
mpirun a.out