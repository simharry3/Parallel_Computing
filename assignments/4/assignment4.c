#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

void generateData(char** data, int size);
void writeData(char** data, char* filename, MPI_File *mpiF);
void readData(char** data, char* filename, MPI_File *mpiF);

// Global Variables!
int mpi_rank; 
int blockSize = 0;
MPI_Status status;

// ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>
int main(int argc, char* argv[]){

	
	int mpi_commsize;

	int numThreads = 0;

	int numReadWrite = 0;
	int numExp = 0;
	//unsigned long long start_cycle = 0, end_cycle = 0, totalRead = 0, totalWrite = 0; 
	double start_cycle = 0, end_cycle = 0, totalRead = 0, totalWrite = 0;
	char* filename  = "carelessWhisper.mov";

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
	


	// call all read and writes
	char* data;
	generateData(&data, blockSize);
	for(int i = 0 ; i < blockSize; ++i){
		printf("%c", data[i]);
	}
	printf("\n");

	/////////////////////////
	// MAIN EXPERIMENT 
	/////////////////////////
	for(int i = 0; i < numExp; ++i){
		if(mpi_rank == 0){
			start_cycle = MPI_Wtime();
		}
		writeData(&data, filename, &mpiF);
		if(mpi_rank == 0){
			totalWrite += MPI_Wtime(); - start_cycle;
		}
		MPI_Barrier(MPI_COMM_WORLD);


		if(mpi_rank == 0){
			start_cycle = MPI_Wtime();
		}
		readData(&data, filename, &mpiF);
		if(mpi_rank == 0){
			totalRead += MPI_Wtime(); - start_cycle;
		}
		MPI_Barrier(MPI_COMM_WORLD);

	}
	/////////////////////////
	// Find the total cycle and close the file

	//All reduce does not work.. but we are told to use it we will figure it out.
	//MPI_Allreduce(total_cycle);
	// if(mpi_rank == 0){
	// 	end_cycle = MPI_Wtime();
	// 	total_cycle = end_cycle - start_cycle;
	// }
	if(mpi_rank == 0){
		printf("%.2f \n", totalWrite);
		printf("%.2f \n", totalRead);

	}

	MPI_File_close(&mpiF);
	MPI_Finalize();
	exit(0);
}


void generateData(char** data, int size){
	(*data) = calloc(size + 1, sizeof(char));
	memset((*data), 'a', size * sizeof(char));
}

void writeData(char** data, char* filename, MPI_File* mpiF){
	MPI_Offset off = mpi_rank * blockSize;
	//ROMIO_CONST void *buf;
	MPI_File_write_at_all(*mpiF, off, *data, blockSize, MPI_CHAR, &status);
}


void readData(char** data, char* filename, MPI_File* mpiF){
	MPI_Offset off = mpi_rank * blockSize;
	//ROMIO_CONST void *buf;
	MPI_File_read_at_all(*mpiF, off, *data, blockSize, MPI_CHAR, &status);
}




// READ ME

//We need to use MPI_File_write_at_all and MPI_File_read_at_all this will be used to 

// here is a Write_at_all link https://www.mpich.org/static/docs/v3.2/www3/MPI_File_write_at_all.html
// we will use the offset to determine where to start the file modification per each rank.
//read_at_all link https://www.mpich.org/static/docs/v3.1/www3/MPI_File_read_at_all.html


// There is still one compile error regarding line 77