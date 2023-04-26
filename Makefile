CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
HASH=hashtable
LISTS=ll_lists
AUX=aux_functions
.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(AUX).o $(HASH).o $(LISTS).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(AUX).o: $(AUX).c
	$(CC) $(CFLAGS) $^ -c

$(LISTS).o: $(LISTS).c
	$(CC) $(CFLAGS) $^ -c

$(HASH).o: $(HASH).c
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
