/***************************************************************************/
/* Template for Asssignment 3 **********************************************/
/* Your Name Here             **********************************************/
/***************************************************************************/

/***************************************************************************/
/* Includes ****************************************************************/
/***************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<math.h>
// Local RNG Include for Assignment 3
#include<clcg4.h>
#include<mpi.h>


/***************************************************************************/
/* Defines *****************************************************************/
/***************************************************************************/

#define ALIVE 1
#define DEAD  0
#define DEBUG 0
//8192
#define WIDTH 8192
#define HEIGHT 8192

#define EVOLVE_TIME 100

#define THRESHOLD 50

#define CHECKPOINT_INTERVAL 5

/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

// You define these
int** universeCheckpoint;


/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

//Board Functions
void initBoard();
void populateBoard();
void populateRow();

void destroyBoard();

//Runtime Functions
void updateCells();
int checkCondition(int*, int);
void updateGhostData(int***, int, int);
void writeCheckpoint(int***, int***, int, int);



/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[])
{
//    int i = 0;
    int mpi_myrank;
    int mpi_commsize;
// Example MPI startup and using CLCG4 RNG
    MPI_Init( &argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_commsize);
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_myrank);
// Init 16,384 RNG streams - each rank has an independent stream
    InitDefault();
    
// Note, used the mpi_myrank to select which RNG stream to use.
// You must replace mpi_myrank with the right row being used.
// This just show you how to call the RNG.    
    
// Insert your code
    int thresh = THRESHOLD;
    if(argc == 2){
        thresh = strtol(argv[1], NULL, 10);
    }
    //printf("THRESHOLD: %d\n", thresh);
    int start = 0;
    if(mpi_myrank == 0){
        start = MPI_Wtime();
    }
    int** gameBoard = NULL;

    universeCheckpoint = calloc(HEIGHT, sizeof(int*));
    for(int i = 0; i < HEIGHT; ++i){
        universeCheckpoint[i] = calloc(WIDTH, sizeof(int));
    }

    initBoard(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize);

    MPI_Barrier( MPI_COMM_WORLD );

    populateBoard(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize, thresh);

    MPI_Barrier( MPI_COMM_WORLD );

    for(int i = 0; i < EVOLVE_TIME; ++i){
        if(DEBUG){
            printf("RANK %d: Timestep: %d\n", mpi_myrank, i);
        }
        updateGhostData(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize);
        MPI_Barrier(MPI_COMM_WORLD); //barrier here to make sure all ghost data is up to date
        updateCells(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize, thresh);
        MPI_Barrier(MPI_COMM_WORLD);
        if(DEBUG){
            printf("=========================\n");
            fflush(NULL);
            MPI_Barrier(MPI_COMM_WORLD);
        }
        if(i % CHECKPOINT_INTERVAL == 0){
            writeCheckpoint(&gameBoard, &universeCheckpoint, mpi_myrank, HEIGHT/mpi_commsize);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }


    MPI_Barrier( MPI_COMM_WORLD );
    destroyBoard(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize);

// END -Perform a barrier and then leave MPI
    MPI_Barrier( MPI_COMM_WORLD );
    double end;
    if(mpi_myrank == 0){
        end = MPI_Wtime();
        printf("RANKS: %d, THRESHOLD: %d, RUNNING TIME: %f\n", mpi_commsize, thresh, end - start);
    }
    MPI_Finalize();
    return 0;
}

/***************************************************************************/
/* Other Functions - You write as part of the assignment********************/
/***************************************************************************/


void initBoard(int*** gameBoard, int rank, int rowsPerRank){
    *gameBoard = calloc(rowsPerRank + 2, sizeof(int*));
    for(int i = 0; i < rowsPerRank + 2; ++i){
        if(DEBUG){
            printf("RANK %d: INIT ROW %d\n", rank, rank * rowsPerRank + i);
        }
        (*gameBoard)[i] = calloc(WIDTH, sizeof(int));
    }
}

void destroyBoard(int*** gameBoard, int rank, int rowsPerRank){
    for(int i = 0; i < rowsPerRank + 2; ++i){
        if(DEBUG){
            printf("RANK %d: DESTROY ROW %d\n", rank, rank * rowsPerRank + i);
            fflush(NULL);
        }
       free((*gameBoard)[i]);
    }
    free(*gameBoard);
}


void populateBoard(int*** gameBoard, int rank, int rowsPerRank, int threshold){
    for(int i = 1; i < rowsPerRank; ++i){
        if(DEBUG){
            printf("RANK %d: POPULATING ROW %d\n", rank, rank * rowsPerRank + i);
        }
        for(int j = 0; j < WIDTH; ++j){
            (*gameBoard)[i][j] = (GenVal(rank * rowsPerRank + i) * 100 > threshold) ? ALIVE : DEAD;
        }
    }
}

int wrapHorizontalIndex(int index){
    if(index == -1){
        return WIDTH - 1;
    }
    else if(index == WIDTH){
        return 0;
    }
    return index;
}

int wrapRank(int rank, int rowsPerRank){
    if(rank == -1){
        return HEIGHT/rowsPerRank - 1;
    }
    else if(rank == HEIGHT/rowsPerRank){
        return 0;
    }
    return rank;
}

void updateCells(int*** gameBoard, int rank, int rowsPerRank, int threshold){
    int* neighbors = calloc(8, sizeof(int));
    for(int i = 1; i < rowsPerRank; ++i){
        for(int j = 0; j < WIDTH; ++j){
            //Check randomness threshold here
            if(GenVal(rank * rowsPerRank + i) * 100 > threshold){
                int index = 0;
                for(int k = - 1; k <= 1; ++k){
                    for(int l = - 1; l <= 1; ++l){
                        if(k == 0 && l == 0){
                            ++l;
                        }
                        ++index;
                        neighbors[index] = (*gameBoard)[i+k][wrapHorizontalIndex(j+l)];
                    }
                }
                (*gameBoard)[i][j] = checkCondition(neighbors, (*gameBoard)[i][j]);
            }
            else{
                if(GenVal(rank * rowsPerRank + i) > 0.5){
                    (*gameBoard)[i][j] = DEAD;
                }
                else{
                    (*gameBoard)[i][j] = ALIVE;
                }
            }
        }
    }
}

//Small helper function to sum up arrays and clean up the code.
int sumArray(int* array, int size){
    int tempVal = 0;
    for(int i = 0; i < size; ++i){
        tempVal += array[i];
    }
    return tempVal;
}

//Checks the conditions for the cell in the middle of its 8 neighbors.
int checkCondition(int* neighbors, int condition){
    int asum = sumArray(neighbors, 8);
    if(condition == ALIVE){
        if(!(asum < 2 || asum > 3)){
            return ALIVE;
        }
    }
    else{
        if(asum == 3){
            return ALIVE;
        }
    }
    return DEAD;
}

void updateGhostData(int*** gameBoard, int rank, int rowsPerRank){
    MPI_Request request[4];
    if(DEBUG){
        printf("RANK %d: STARTING GDU. Forward Rank: %d Backward Rank: %d\n"
            ,rank, wrapRank(rank + 1, rowsPerRank), wrapRank(rank - 1, rowsPerRank));
        fflush(NULL);
    }
        
    MPI_Isend((*gameBoard)[1], WIDTH, MPI_INT, wrapRank(rank - 1, rowsPerRank),
        rank * rowsPerRank, MPI_COMM_WORLD, &request[0]);
    MPI_Isend((*gameBoard)[rowsPerRank - 1], WIDTH, MPI_INT, wrapRank(rank + 1, rowsPerRank),
        rank * rowsPerRank, MPI_COMM_WORLD, &request[1]);

    MPI_Irecv((*gameBoard)[0], WIDTH, MPI_INT, wrapRank(rank - 1, rowsPerRank), 
        rank * rowsPerRank, MPI_COMM_WORLD, &request[2]);
    MPI_Irecv((*gameBoard)[rowsPerRank], WIDTH, MPI_INT, wrapRank(rank + 1, rowsPerRank),
        rank * rowsPerRank, MPI_COMM_WORLD, &request[3]);
}

void writeCheckpoint(int*** gameBoard, int*** checkpointBoard, int rank, int rowsPerRank){
    if(DEBUG){
        printf("RANK %d: WRITING CHECKPOINT\n", rank);
        fflush(NULL);
    }
    for(int i = 1; i < rowsPerRank; ++i){
        for(int j = 0; j < WIDTH; ++j){ 
            (*checkpointBoard)[rank * rowsPerRank + i][j] = (*gameBoard)[i][j];
        }
    }   
}