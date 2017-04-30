all: types.h types.c operations.h operations.c simulation.c 
	mpicc -I. -Wall -O3 -c types.c -o types.o
	mpicc -I. -Wall -O3 -c operations.c -o operations.o
	mpicc -I. -Wall -O3 simulation.c types.o operations.o -o simulation 