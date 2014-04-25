CFLAGS := -Wall -lm -pthread

default: all

all: bathroomSim bathroom.o
	
bathroomSim: bathroomSim.c bathroom.h bathroom.o
	cc -o bathroomSim bathroomSim.c bathroom.o $(CFLAGS)

bathroom.o: bathroom.h bathroom.c
	cc -c bathroom.c $(CFLAGS)
	
	
clean:
	rm *.o bathroomSim
