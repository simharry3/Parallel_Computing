#include <stdio.h>
#include <mpi.h>

int* generateData(int length);

// ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>
int main(int argc, char* argv[]){

	int mpi_rank; 
	int mpi_commsize;

	if(argc == 5){
		int numThreads = strtol(argv[1]);
		int numBlocks = strtol(argv[2]);
		int numReadWrite = strtol(argv[3]);
		int numExp = strtol(argv[4]);
	}
	else{
		printf("USAGE: ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>\n");
		fflush(NULL);
		return 0;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);





	return 1;
}

