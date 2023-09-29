#!/bin/bash

#SBATCH --partition=full

#SBATCH --job-name=IMT2112

#SBATCH --output=log_con_5_trabajador.out

#SBATCH --ntasks=5
#SBATCH --cpus-per-task=1

mpic++ tarea2_read_matrix.cpp  -std=c++11
time mpirun a.out