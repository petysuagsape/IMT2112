#!/bin/bash

#SBATCH --partition=full

#SBATCH --job-name=IMT2112
#SBATCH --output=log_con_20_trabajador.out

#SBATCH --ntasks=20
#SBATCH --cpus-per-task=1

mpic++ tarea2_read_matrix.cpp  -std=c++11
time mpirun a.out