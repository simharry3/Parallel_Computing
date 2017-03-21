#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]){

	int mpi_rank; 
	int mpi_commsize
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	
	return 1;
}

