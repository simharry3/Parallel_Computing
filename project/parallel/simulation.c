#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "types.h"
#include "operations.h"

int main(int argc, char* argv[]){
    
    state* st;
    context* ctx;

    int mpi_rank;
    int mpi_commsize;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    //<NUM PARTICLES> <NUM STEPS> <chkFreq> <humanOutput> <inputFile>
    int* data = calloc(argc - 1, sizeof(int));
    int i;
    for(i = 1; i < argc; ++i){
        data[i - 1] = strtol(argv[i], NULL, 10);
    }

    // Create a MPI type for struct particle
    int num_blocks_vec3d = 3;
    int blocklengths_vec3d[3] = {1,1,1};
    MPI_Datatype mpi_vec3d;
    MPI_Datatype types_vec3d[3] = {MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offset_vec3d[3];
    offset_vec3d[0] = offsetof(vec3D, x);
    offset_vec3d[1] = offsetof(vec3D, y);
    offset_vec3d[2] = offsetof(vec3D, z);
    MPI_Type_create_struct(num_blocks_vec3d, blocklengths_vec3d, offset_vec3d, types_vec3d, &mpi_vec3d);
    MPI_Type_commit(&mpi_vec3d);


    int num_blocks = 5;
    int blocklengths[3] = {3,1,1};
    MPI_Datatype mpi_particle;
    // May have to create another datatype for vec3D
    MPI_Datatype types[5] = {mpi_vec3d, MPI_UNSIGNED, MPI_INT};
    MPI_Aint offset[3];
    offset[0] = offsetof(particle, position);
    offset[1] = offsetof(particle, id);
    offset[2] = offsetof(particle, collision);

    MPI_Type_create_struct(num_blocks, blocklengths, offset, types, &mpi_particle);
    MPI_Type_commit(&mpi_particle);
    
    initContext(&ctx, data, mpi_commsize);
    initState(&st, ctx);
    initAggregators(st, argv[argc-1], mpi_rank);
    // if(mpi_rank == 0)
    //     printState(st, ctx);
    runSystem(st, ctx, mpi_rank);
    if(mpi_rank == 0){
        printSimulationResults(st, ctx);
    }
    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
    exit(0);
}

