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
#define DEBUG 1
//8192
#define WIDTH 16
#define HEIGHT 16

#define EVOLVE_TIME 10

#define THRESHOLD 50

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
int checkCondition(int* neighbors, int condition);


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
    int** gameBoard = NULL;
    int** ghostRows = NULL;

    initBoard(&gameBoard, &ghostRows, mpi_myrank, HEIGHT/mpi_commsize);

    MPI_Barrier( MPI_COMM_WORLD );

    populateBoard(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize);

    MPI_Barrier( MPI_COMM_WORLD );

    for(int i = 0; i < EVOLVE_TIME; ++i){
        if(DEBUG){
            printf("RANK %d: Timestep: %d\n", mpi_myrank, i);
        }
        updateCells(&gameBoard, mpi_myrank, HEIGHT/mpi_commsize);
        MPI_Barrier(MPI_COMM_WORLD);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    destroyBoard(&gameBoard, &ghostRows, mpi_myrank, HEIGHT/mpi_commsize);

// END -Perform a barrier and then leave MPI
    MPI_Barrier( MPI_COMM_WORLD );

    MPI_Finalize();
    return 0;
}

/***************************************************************************/
/* Other Functions - You write as part of the assignment********************/
/***************************************************************************/


void initBoard(int*** gameBoard, int*** ghostRows, int rank, int rowsPerRank){
    *ghostRows = calloc(2, sizeof(int*));
    for(int i = 0; i < 2; ++i){
        (*ghostRows)[i] = calloc(WIDTH, sizeof(int));
    }

    *gameBoard = calloc(rowsPerRank, sizeof(int*));
    for(int i = 0; i < rowsPerRank + 2; ++i){
        if(DEBUG){
            printf("RANK %d: INIT ROW %d\n", rank, rank * rowsPerRank + i);
        }
        (*gameBoard)[i] = calloc(WIDTH, sizeof(int));
    }
}

void destroyBoard(int*** gameBoard, int*** ghostRows, int rank, int rowsPerRank){
    for(int i = 0; i < 2; ++i){
        free((*ghostRows)[i]);
    }
    free(*ghostRows);
    
    for(int i = 0; i < rowsPerRank + 2; ++i){
        if(DEBUG){
            printf("RANK %d: DESTROY ROW %d\n", rank, rank * rowsPerRank + i);
        }
       free((*gameBoard)[i]);
    }
    free(*gameBoard);
}


void populateBoard(int*** gameBoard, int rank, int rowsPerRank){
    for(int i = 1; i < rowsPerRank; ++i){
        if(DEBUG){
            printf("RANK %d: POPULATING ROW %d\n", rank, rank * rowsPerRank + i);
        }
        for(int j = 0; j < WIDTH; ++j){
            (*gameBoard)[i][j] = (GenVal(rank * rowsPerRank + i) > THRESHOLD) ? 1 : 0;
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

void updateCells(int*** gameBoard, int rank, int rowsPerRank){
    int* neighbors = calloc(8, sizeof(int));
    for(int i = 1; i < rowsPerRank; ++i){
        for(int j = 0; j < WIDTH; ++j){
            //Check randomness threshold here
            if(GenVal(rank * rowsPerRank + i) > THRESHOLD){
                int index = 0;
                for(int k = - 1; k <= 1; ++k){
                    for(int l = 0; l <= 1; ++l){
                        if(k == 0 || l == 0){
                            ++l;
                        }
                        neighbors[index] = (*gameBoard[i+k][wrapHorizontalIndex(j+l)]);
                    }
                }
                (*gameBoard)[i][j] = checkCondition(neighbors, (*gameBoard)[i][j]);
            }
            else{
                (*gameBoard)[i][j] = DEAD;
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