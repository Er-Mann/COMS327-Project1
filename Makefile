CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2


all: mapper

mapper: mapper.o data.o
	$(CC) $(CFLAGS) -o mapper mapper.o data.o

mapper.o: mapper.c data.h
	$(CC) $(CFLAGS) -c mapper.c

data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c

clean:
	rm -f mapper *.o