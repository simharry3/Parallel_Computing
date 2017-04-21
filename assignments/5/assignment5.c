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
#include<pthread.h>


/***************************************************************************/
/* Defines *****************************************************************/
/***************************************************************************/

#define ALIVE 1
#define DEAD  0
#define DEBUG 0
#define HUMAN_OUTPUT 0
//8192
#define WIDTH 1024
#define HEIGHT 1024

#define DEFAULT_PTHREADS 1
#define DEFAULT_NODES 1

#define EVOLVE_TIME 100

#define THRESHOLD 50

#define CHECKPOINT_INTERVAL 5

/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

// You define these
typedef struct{
    int** cellData;
    int threshold;
    int pThreadsPerNode;
    int nNodes;
    pthread_mutex_t* mutex;
}gameData;

gameData* gData;

gameData universeCheckpoint;

pthread_barrier_t barrier;

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

//Board Functions
void* initBoard();
void* populateBoard();
void* destroyBoard();

//Runtime Functions
void* updateCells();
int checkCondition(int*, int);
void* updateGhostData(void*, int);
void* writeCheckpoint(void*);
void* xzibit(void*);



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

    ////////////////////////////////////
    //USER INPUT TO STREAMLINE TESTING:
    int thresh = THRESHOLD;
    int numThreads = DEFAULT_PTHREADS;
    int nodes = DEFAULT_NODES;
    if(argc == 4){
        thresh = strtol(argv[1], NULL, 10);
        numThreads = strtol(argv[2], NULL, 10);
        nodes = strtol(argv[3], NULL, 10);

    }
    ///////////////////////////////////

    int start = 0;
    if(mpi_myrank == 0){
        start = MPI_Wtime();
    }
    gData = calloc(1, sizeof(gData));
    gData->threshold = thresh;
    gData->pThreadsPerNode = numThreads; 
    gData->nNodes = nodes;

    universeCheckpoint.cellData = calloc(HEIGHT, sizeof(int*));
    for(int i = 0; i < HEIGHT; ++i){
        universeCheckpoint.cellData[i] = calloc(WIDTH, sizeof(int));
    }


    if(numThreads != 0){
        ////////////////////////////////////
        //Initialize Pthread Barrier:
        int rc;
        rc = pthread_barrier_init(&barrier, NULL, numThreads);
        ////////////////////////////////////

        ////////////////////////////////////
        //Initialize Pthreads:
        pthread_t* threads = calloc(numThreads, sizeof(pthread_t));
        int* threadNums = calloc(numThreads, sizeof(int));
        for(int i = 0; i < numThreads; ++i){
            // printf("Creating Pthread %d\n", i);
            threadNums[i] = i;
            fflush(NULL);
            rc = pthread_create(&threads[i], NULL, xzibit, (void*) &(threadNums[i]));
            if(rc){
                printf("ERROR: %d IN PTHREAD CREATE\n", rc);
                exit(-1);
            }
        }
        /////////////////////////////////////

        for(int i = 0; i < numThreads; ++i){
            rc = pthread_join(threads[i], NULL);
            if(1){
                // printf("JOINED THREAD %d\n", threadNums[i]);
                fflush(NULL);
            }
            if(rc){
                printf("ERROR %d IN PTHREAD JOIN\n", rc);
                fflush(NULL);
                exit(-1);
            }
        }
    }
    else{
        xzibit(&numThreads);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end;
    if(mpi_myrank == 0){
        end = MPI_Wtime();
        if(HUMAN_OUTPUT){
            printf("RANKS: %d, THRESHOLD: %d, RUNNING TIME: %f\n", mpi_commsize, thresh, end - start);
        }
        else{
            printf("%d %d %d %d %f\n", thresh, gData->pThreadsPerNode, mpi_commsize, gData->nNodes, end - start);
        }
    }
    
    MPI_Finalize();
    if(numThreads != 0){
        pthread_exit(NULL);
    }
    else{
        exit(0);
    }
}

/***************************************************************************/
/* Other Functions - You write as part of the assignment********************/
/***************************************************************************/


void* initBoard(void* arg, int pid){
    gameData** gData = (gameData**)arg;
    int rank, commSize, rowsPerRank, rowsPerThread;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    rowsPerRank = HEIGHT/commSize;
    if((*gData)->pThreadsPerNode != 0){
        rowsPerThread = rowsPerRank/(*gData)->pThreadsPerNode;
    }
    else{
        rowsPerThread = rowsPerRank;
    }

    // printf("RPT: %d H: %d cS: %d\n", rowsPerThread, HEIGHT, commSize);
    // printf("RANK %d THREAD %d: PRE BARRIER\n", rank, pid);
    // fflush(NULL);    
    if(pid == 0){
        (*gData)->cellData = calloc(rowsPerRank + 2, sizeof(int*));
        (*gData)->cellData[0] = calloc(WIDTH, sizeof(int));
        (*gData)->cellData[rowsPerRank + 1] = calloc(WIDTH, sizeof(int));
        // printf("RANK %d THREAD %d: INIT GHOST ROW %d\n", rank, pid, 0);
        // printf("RANK %d THREAD %d: INIT GHOST ROW %d\n", rank, pid, rowsPerRank + 1);
        fflush(NULL);
    }
    if((*gData)->pThreadsPerNode != 0){
        pthread_barrier_wait(&barrier);
    }
    // printf("RANK %d THREAD %d: POST BARRIER\n", rank, pid);
    // fflush(NULL);

    for(int i = rowsPerThread * pid + 1; i < rowsPerThread * pid + rowsPerThread + 1; ++i){
        if(DEBUG){
            printf("RANK %d THREAD %d: INIT ROW %d\n", rank, pid, rank * rowsPerRank + i);
            fflush(NULL);
        }
        (*gData)->cellData[i] = calloc(WIDTH, sizeof(int));
    }

    return NULL;
}

void* destroyBoard(void* arg, int pid){
    gameData** gData = (gameData**)arg;
    int rank, commSize, rowsPerRank, rowsPerThread;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    rowsPerRank = HEIGHT/commSize;
    if((*gData)->pThreadsPerNode != 0){
        rowsPerThread = rowsPerRank/(*gData)->pThreadsPerNode;
    }
    else{
        rowsPerThread = rowsPerRank;
    }

    for(int i = rowsPerThread * pid + 1; i < rowsPerThread * pid + rowsPerThread + 1; ++i){
        if(DEBUG){
            printf("RANK %d THREAD %d: DESTROY ROW %d\n", rank, pid, rank * rowsPerRank + i);
            fflush(NULL);
        }
       free((*gData)->cellData[i]);
    }
    if((*gData)->pThreadsPerNode != 0){
        pthread_barrier_wait(&barrier);
    }
    if(pid == 0){
        free((*gData)->cellData[0]);
        free((*gData)->cellData[rowsPerRank + 1]);
        free(*gData);
    }
    return NULL;
}


void* populateBoard(void* arg, int pid){
    gameData** gData = (gameData**)arg;
    int rank, commSize, rowsPerRank, rowsPerThread;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    rowsPerRank = HEIGHT/commSize;
    if((*gData)->pThreadsPerNode != 0){
        rowsPerThread = rowsPerRank/(*gData)->pThreadsPerNode;
    }
    else{
        rowsPerThread = rowsPerRank;
    }

    for(int i = rowsPerThread * pid + 1; i < rowsPerThread * pid + rowsPerThread + 1; ++i){
        if(DEBUG){
            printf("RANK %d: POPULATING ROW %d\n", rank, rank * rowsPerRank + i);
            fflush(NULL);
        }
        for(int j = 0; j < WIDTH; ++j){
            (*gData)->cellData[i][j] = (GenVal(rank * rowsPerRank + i) * 100 > (*gData)->threshold) ? ALIVE : DEAD;
        }
    }
    return NULL;
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

void pthread_mpi_barrier(int pid){
    if(pid == 0){
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if(gData->pThreadsPerNode != 0){
        pthread_barrier_wait(&barrier);
    }
}

void* updateCells(void* arg, int pid){
    gameData** gData = (gameData**)arg;
    int rank, commSize, rowsPerRank, rowsPerThread;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    rowsPerRank = HEIGHT/commSize;
    if((*gData)->pThreadsPerNode != 0){
        rowsPerThread = rowsPerRank/(*gData)->pThreadsPerNode;
    }
    else{
        rowsPerThread = rowsPerRank;
    }

    int* neighbors = calloc(8, sizeof(int));
    for(int i = rowsPerThread * pid + 1; i < rowsPerThread * pid + rowsPerThread + 1; ++i){
        for(int j = 0; j < WIDTH; ++j){
            //Check randomness threshold here
            if(GenVal(rank * rowsPerRank + i) * 100 > (*gData)->threshold){
                int index = 0;
                for(int k = - 1; k <= 1; ++k){
                    for(int l = - 1; l <= 1; ++l){
                        if(k == 0 && l == 0){
                            ++l;
                        }
                        ++index;
                        neighbors[index] = (*gData)->cellData[i+k][wrapHorizontalIndex(j+l)];
                    }
                }
                (*gData)->cellData[i][j] = checkCondition(neighbors, (*gData)->cellData[i][j]);
            }
            else{
                if(GenVal(rank * rowsPerRank + i) > 0.5){
                    (*gData)->cellData[i][j] = DEAD;
                }
                else{
                    (*gData)->cellData[i][j] = ALIVE;
                }
            }
        }
    }
    return NULL;
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

void* updateGhostData(void* arg, int pid){
    gameData** gData = (gameData**)arg;
    int rank, commSize, rowsPerRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    rowsPerRank = HEIGHT/commSize;

    MPI_Request request[4];
    if(DEBUG){
        printf("RANK %d: STARTING GDU. Forward Rank: %d Backward Rank: %d\n"
            ,rank, wrapRank(rank + 1, rowsPerRank), wrapRank(rank - 1, rowsPerRank));
        fflush(NULL);
    }

    if(pid == 0){    
        MPI_Isend((*gData)->cellData[1], WIDTH, MPI_INT, wrapRank(rank - 1, rowsPerRank),
            rank * rowsPerRank, MPI_COMM_WORLD, &request[0]);
        MPI_Isend((*gData)->cellData[rowsPerRank - 1], WIDTH, MPI_INT, wrapRank(rank + 1, rowsPerRank),
            rank * rowsPerRank, MPI_COMM_WORLD, &request[1]);

        pthread_mpi_barrier(pid);

        MPI_Irecv((*gData)->cellData[0], WIDTH, MPI_INT, wrapRank(rank - 1, rowsPerRank), 
            rank * rowsPerRank, MPI_COMM_WORLD, &request[2]);
        MPI_Irecv((*gData)->cellData[rowsPerRank], WIDTH, MPI_INT, wrapRank(rank + 1, rowsPerRank),
            rank * rowsPerRank, MPI_COMM_WORLD, &request[3]);
    }
    return NULL;
}

void* writeCheckpoint(void* arg){
    gameData** cData = (gameData**)arg;
    int rank, commSize, rowsPerRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    rowsPerRank = HEIGHT/commSize;

    if(DEBUG){
        printf("RANK %d: WRITING CHECKPOINT\n", rank);
        fflush(NULL);
    }
    for(int i = 1; i < rowsPerRank; ++i){
        for(int j = 0; j < WIDTH; ++j){ 
            (*cData)->cellData[rank * rowsPerRank + i][j] = (*cData)->cellData[i][j];
        }
    }   
    return NULL;
}


//Main wrapper function for pthreads. Xzibit. Get it? haha. ha. 
void* xzibit (void* arg){
    int* temp = (int*)arg;
    int pid = *temp;
    // printf("MY PID IS: %d\n", pid);
    fflush(NULL);
    int rank, commSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    

    initBoard(&gData, pid);

    //Synchronize threads after initialization of the game board struct
    // MPI_Barrier( MPI_COMM_WORLD );
    pthread_mpi_barrier(pid);

    populateBoard(&gData, pid);

    //Synchronize threads after population of game boards
    pthread_mpi_barrier(pid);
    // MPI_Barrier( MPI_COMM_WORLD );


    // MAIN LOOP WHICH RUNS THE PROGRAM:
    for(int i = 0; i < EVOLVE_TIME; ++i){
        if(DEBUG){
            printf("RANK %d: Timestep: %d\n", rank, i);
            fflush(NULL);
        }
        updateGhostData(&gData, pid);
        pthread_mpi_barrier(pid);
        // MPI_Barrier(MPI_COMM_WORLD);
        // MPI_Barrier(MPI_COMM_WORLD); //barrier here to make sure all ghost data is up to date
        updateCells(&gData, pid);
        // MPI_Barrier(MPI_COMM_WORLD);
        if(DEBUG){
            printf("=========================\n");
            fflush(NULL);
            // MPI_Barrier(MPI_COMM_WORLD);
        }

        pthread_mpi_barrier(pid);
        // MPI_Barrier(MPI_COMM_WORLD);
        // //CHECKPOINTING:
        // if(i % CHECKPOINT_INTERVAL == 0){
        //     writeCheckpoint(&universeCheckpoint);
        //     MPI_Barrier(MPI_COMM_WORLD);
        // }
    }


    pthread_mpi_barrier(pid);
    // MPI_Barrier(MPI_COMM_WORLD);
    destroyBoard(&gData, pid);

    pthread_mpi_barrier(pid);
    if(gData->pThreadsPerNode != 0){
        pthread_exit(NULL);
    }
    else{
        return NULL;
    }
}