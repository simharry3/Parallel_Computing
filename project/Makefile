CC = g++
CFLAGS = -I.
DEPS = particle.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: particle.o simulation.o 
	g++ -o simulation simulation.o particle.o $(CFLAGS)