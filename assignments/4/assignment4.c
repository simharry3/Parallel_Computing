#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

void generateData(int**, int);

// ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>
int main(int argc, char* argv[]){

	int mpi_rank; 
	int mpi_commsize;

	int numThreads = 0;
	int blockSize = 0;
	int numReadWrite = 0;
	int numExp = 0;

	if(argc == 5){
		numThreads = strtol(argv[1], NULL, 10);
		blockSize = strtol(argv[2], NULL, 10);
		numReadWrite = strtol(argv[3], NULL, 10);
		numExp = strtol(argv[4], NULL, 10);
	}
	else{
		printf("USAGE: ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>\n");
		fflush(NULL);
		return 0;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	int* data;
	generateData(&data, blockSize);
	for(int i = 0 ; i < blockSize; ++i){
		printf("%c", data[i]);
	}
	printf("\n");




	return 1;
}


void generateData(int** data, int size){
	printf("SIZEOF CHAR: %d\n", sizeof(char));
	(*data) = calloc(size + 1, sizeof(char));
	for(int i = 0; i < size; ++i){
		printf("%d\n", i);
		data[i] = 'a';
	}
	// memset(temp, 'a', size * sizeof(char));
}
