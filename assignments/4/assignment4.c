#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

void generateData(int** data, int size);

// ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>
int main(int argc, char* argv[]){

	int mpi_rank; 
	int mpi_commsize;

	int numThreads = 0;
	int blockSize = 0;
	int numReadWrite = 0;
	int numExp = 0;
	unsigned int long long start_cycle =0, end_cycle = 0, total_cycle = 0;


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

	// FILESIZE determines the size of the overall file in relation to the blocksize and commsize
	int FILESIZE = blockSize * mpi_commsize;
	

	// The three lines below may not be used
	//int buf[FILESIZE];
	//bufsize = FILESIZE/nprocs;
	//nints = bufsize/sizeof(int);

	//Open the file and record start time
	MPI_File mpiF;
	MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDONLY, MPI_INFO_NULL, &mpiF);
	//start_cycle = GetTimeBase();


	// call all read and writes
	int* data;
	generateData(&data, blockSize);
	for(int i = 0 ; i < blockSize; ++i){
		printf("%c", data[i]);
	}
	printf("\n");


	// Find the total cycle and close the file
	//end_cycle = GetTimeBase();
	//total_cycle = end_cycle - start_cycle;
	//All reduce does not work.. but we are told to use it we will figure it out.
	//MPI_Allreduce(total_cycle);
	MPI_File_close(&mpiF);

	return 1;
}


void generateData(int** data, int size){
	printf("SIZEOF CHAR: %d\n", (int)sizeof(char));
	(*data) = calloc(size + 1, sizeof(char));
	for(int i = 0; i < size; ++i){
		printf("%d\n", i);
		data[i] = 'a';
	}
	// memset(temp, 'a', size * sizeof(char));
}




// READ ME

//We need to use MPI_File_write_at_all and MPI_File_read_at_all this will be used to 

// here is a Write_at_all link https://www.mpich.org/static/docs/v3.2/www3/MPI_File_write_at_all.html
// we will use the offset to determine where to start the file modification per each rank.
//read_at_all link https://www.mpich.org/static/docs/v3.1/www3/MPI_File_read_at_all.html


// There is still one compile error regarding line 77