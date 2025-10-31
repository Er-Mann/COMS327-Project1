CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2


all: mapper testgraph citydata

mapper: mapper.o data.o
	$(CC) $(CFLAGS) -o mapper mapper.o data.o

mapper.o: mapper.c data.h
	$(CC) $(CFLAGS) -c mapper.c

data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c



testgraph: testgraph.o graph.o
	$(CC) $(CFLAGS) -o testgraph testgraph.o graph.o

testgraph.o: testgraph.c testgraph.h graph.h
	$(CC) $(CFLAGS) -c testgraph.c

graph.o: graph.c graph.h
	$(CC) $(CFLAGS) -c graph.c



citydata: citydata.o graph.o
	$(CC) $(CFLAGS) -o citydata citydata.o graph.o

citydata.o: citydata.c citydata.h graph.h testgraph.h
	$(CC) $(CFLAGS) -c citydata.c


clean:
	rm -f mapper testgraph citydata *.o