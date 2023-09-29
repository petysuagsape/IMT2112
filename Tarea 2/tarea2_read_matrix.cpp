// Elwin van t Wout
// Pontificia Universidad Católica de Chile
// 9-9-2023

// Read a matrix from a text file and store specific lines in an array.

#include <iostream>
#include <fstream>
#include <mpi.h>
#include <cmath>

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int nrows, ncols;
    double *my_matrix;
    double tmp;

    int world_size, world_rank;
    int start_row, end_row;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int rows_per_process;
    int remaining_rows;

    ifstream file;

    file.open("matrix.txt");




    if (file.is_open())
    {
        file >> nrows;
        
        rows_per_process = nrows / world_size;
        remaining_rows = nrows % world_size;
        // Calcular el rango de filas para cada proceso
        // proceso 0 tiene más columnas y todos los otros tienen las mismas columnas
        if (world_rank == 0 ) {
            start_row = 0;
            end_row = rows_per_process + remaining_rows-1;
        } else {
            start_row = world_rank * rows_per_process + remaining_rows;
            end_row = start_row + (rows_per_process-1);
        }

        file >> ncols;
        int my_firstrow = start_row;
        int my_nrows = end_row-start_row+1;

        my_matrix = new double [my_nrows * ncols];

        for (int i=0; i<(start_row)*ncols; i++) {
            file >> tmp;
        }
        
        for (int i=0; i<my_nrows*ncols; i++) {
            file >> my_matrix[i];
        }

        file.close();
    }
    else
    {
        cout << "Unable to open file." << endl;
    }

    
    double*b = new double[end_row - start_row + 1];
    double*b_all = new double[ncols];
    double*Ab = new double[ncols];
    double*b_resto = new double[remaining_rows];
    double*b_0_sin_resto = new double[rows_per_process];

    double*b_igual = new double[rows_per_process];
    double*received_b = new double[rows_per_process*(world_size)];
    double*valor_b = new double[end_row - start_row + 1];

    

    for (int i = 0; i < (end_row-start_row+1); i++) {
        b[i] = 1.0;
    }



//////////////////////////////////////////////////////////////////////////////////////////////////////




    // ahora implementaremos el metodo de potencia
    for (int z = 0; z < 10000; z++) {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////// Aquí juntamos el vector b, este codigo es para juntar el b en b_all  bien/////////////////////////////////////////////////      
        if (world_rank == 0 ) {
            for (int i = remaining_rows; i < (end_row-start_row+1); i++) {
                b_igual[i-remaining_rows] = b[i];}
            for (int i = 0; i < (remaining_rows); i++) {
                b_resto[i] = b[i];    }}
        else { b_igual = b;  }

        MPI_Allgather(b_igual,rows_per_process, MPI_DOUBLE, received_b, rows_per_process, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Bcast(b_resto, remaining_rows, MPI_COUNT, 0, MPI_COMM_WORLD);

        for (int i=remaining_rows; i<ncols;i++){
            b_all[i] = received_b[i-remaining_rows];}

        for (int i=0; i<remaining_rows;i++){
            b_all[i] = b_resto[i];}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// este codigo es para actualizar el b bien/////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < end_row - start_row + 1; i++) {
            valor_b[i] = 0.0;
            for (int j = 0; j < nrows; j++) {
                valor_b[i] += b_all[j]*my_matrix[i*ncols + j]; }
            b[i] = valor_b[i];   }
            
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Normalizaremos el vector b /////////////////////////////////////////////////////////////////////////////////////////
        if (world_rank == 0 ) {
            for (int i = remaining_rows; i < (end_row-start_row+1); i++) {
                b_igual[i-remaining_rows] = b[i];}
            for (int i = 0; i < (remaining_rows); i++) {
                b_resto[i] = b[i];    }
                } else { b_igual = b;  }

        MPI_Allgather(b_igual,rows_per_process, MPI_DOUBLE, received_b, rows_per_process, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Bcast(b_resto, remaining_rows, MPI_COUNT, 0, MPI_COMM_WORLD);

        for (int i=remaining_rows; i<ncols;i++){
            b_all[i] = received_b[i-remaining_rows];}

        for (int i=0; i<remaining_rows;i++){
            b_all[i] = b_resto[i];}

        double norma = 0.0;
        for (int i = 0; i < ncols; i++) {
            norma += b_all[i] * b_all[i];
        }
        for (int i = 0; i < end_row - start_row + 1; i++) {
            b[i] = b[i]/std::sqrt(norma);}
    }
    
            
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Cuando termina la iteración calcularemos el máximo valor propio ////////////////////////////////////
    if (world_rank == 0 ) {
            for (int i = remaining_rows; i < (end_row-start_row+1); i++) {
                b_igual[i-remaining_rows] = b[i];}
            for (int i = 0; i < (remaining_rows); i++) {
                b_resto[i] = b[i];    }
                } else { b_igual = b;  }

        MPI_Allgather(b_igual,rows_per_process, MPI_DOUBLE, received_b, rows_per_process, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Bcast(b_resto, remaining_rows, MPI_COUNT, 0, MPI_COMM_WORLD);

        for (int i=remaining_rows; i<ncols;i++){
            Ab[i] = received_b[i-remaining_rows];}

        for (int i=0; i<remaining_rows;i++){
            Ab[i] = b_resto[i];}


    for (int i = 0; i <  end_row - start_row + 1; i++) {
        valor_b[i] = 0.0;
        for (int j = 0; j < ncols; j++) {
            valor_b[i] += my_matrix[i*ncols + j]*Ab[j]; }
        b[i] = valor_b[i];   }


    if (world_rank == 0 ) {
            for (int i = remaining_rows; i < (end_row-start_row+1); i++) {
                b_igual[i-remaining_rows] = b[i];}
            for (int i = 0; i < (remaining_rows); i++) {
                b_resto[i] = b[i];    }
                } else { b_igual = b;  }

        MPI_Allgather(b_igual,rows_per_process, MPI_DOUBLE, received_b, rows_per_process, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Bcast(b_resto, remaining_rows, MPI_COUNT, 0, MPI_COMM_WORLD);

        for (int i=remaining_rows; i<ncols;i++){
            b_all[i] = received_b[i-remaining_rows];}

        for (int i=0; i<remaining_rows;i++){
            b_all[i] = b_resto[i];}


    double valor_propio = 0.0;
    for (int i=0; i<ncols;i++){
        valor_propio += Ab[i]*b_all[i];
    }
    
    
    cout << "Proceso " << world_rank << " el valor propio es " << valor_propio << endl;
    





//////////////////////////////////////////////////////////////////////////////////////////////////////
     
    


    // delete[] b;
    // delete[] b_all;
    // delete[] Ab;
    // delete[] b_resto;
    // delete[] b_0_sin_resto;
    // delete[] b_igual;
    // delete[] received_b;
    // delete[] valor_b;
    // delete[] my_matrix;

    MPI_Finalize();

    return 0;
}






// for (int i = 0; i < end_row - start_row + 1; i++) {
//             cout << "Proceso " << world_rank << " el b normalizado es " << b[i] << endl;}