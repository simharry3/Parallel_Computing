#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void generateData(char** data, int size);
void writeData(char** data, MPI_File *mpiF, int k);
void readData(char** data, MPI_File *mpiF, int k);

// Global Variables!
int mpi_rank; 
int blockSize = 0;
MPI_Status status;
int mpi_commsize;
int numFiles = 0;

// ./a <Num Files> <Block Size> <Num Read/Write> <Num Experiments>
int main(int argc, char* argv[]){

	
	

	int numReadWrite = 0;
	int numExp = 0;
	//unsigned long long start_cycle = 0, end_cycle = 0, totalRead = 0, totalWrite = 0; 
	double start_cycle = 0, end_cycle = 0, totalRead = 0, totalWrite = 0;
	

	if(argc == 5){
		numFiles = strtol(argv[1], NULL, 10);
		blockSize = strtol(argv[2], NULL, 10)*1024*1024;
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


	// call all read and writes
	char* data;
	generateData(&data, blockSize);


	/////////////////////////
	// MAIN EXPERIMENT 
	/////////////////////////
	mkdir("./data", 0700);
	char* filename = calloc(21, sizeof(char));
	int fileInfo = mpi_rank/(mpi_commsize/numFiles);
	sprintf(filename, "./data/datafile%d.dat", fileInfo);

		

	for(int i = 0; i < numExp; ++i){
		remove(filename);
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &mpiF);


		if(mpi_rank == 0){
			start_cycle = MPI_Wtime();
		}
		for(int j = 0; j < numReadWrite; ++j){
			writeData(&data, &mpiF, j);
		}
		if(mpi_rank == 0){
			totalWrite += MPI_Wtime(); - start_cycle;
		}


		MPI_Barrier(MPI_COMM_WORLD);


		if(mpi_rank == 0){
			start_cycle = MPI_Wtime();
		}
		for(int j = 0; j < numReadWrite; ++j){
			readData(&data, &mpiF, j);
		}
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
		printf("Write Time -- Files: %d, Block: %d, Comm: %d, Time: %.2f \n", numFiles, blockSize, mpi_commsize, totalWrite/numExp);
		printf("Read Time -- Files: %d, Block: %d, Comm: %d, Time: %.2f \n", numFiles, blockSize, mpi_commsize, totalRead/numExp);
		fflush(NULL);

	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_File_close(&mpiF);
	MPI_Finalize();
	exit(0);
}


void generateData(char** data, int size){
	(*data) = calloc(size + 1, sizeof(char));
	memset((*data), 'a', size * sizeof(char));
}

void writeData(char** data, MPI_File* mpiF, int k){
	MPI_Offset off = mpi_rank % (mpi_commsize/numFiles) * blockSize + (blockSize*k);
	//ROMIO_CONST void *buf;
	MPI_File_write_at_all(*mpiF, off, *data, blockSize, MPI_CHAR, &status);
}


void readData(char** data, MPI_File* mpiF, int k){
	MPI_Offset off = mpi_rank % (mpi_commsize/numFiles) * blockSize + (blockSize*k);
	//ROMIO_CONST void *buf;
	MPI_File_read_at_all(*mpiF, off, *data, blockSize, MPI_CHAR, &status);
}




// READ ME

//We need to use MPI_File_write_at_all and MPI_File_read_at_all this will be used to 

// here is a Write_at_all link https://www.mpich.org/static/docs/v3.2/www3/MPI_File_write_at_all.html
// we will use the offset to determine where to start the file modification per each rank.
//read_at_all link https://www.mpich.org/static/docs/v3.1/www3/MPI_File_read_at_all.html


// There is still one compile error regarding line 77