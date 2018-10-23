CC=gcc
FLAGS=-std=c99
BASENAME=bitwise_util

all: O0 O1 O2 O3 Os Ofast

O0:
	$(CC) $(FLAGS) -O0 $(BASENAME).c -o $(BASENAME).0 
O1:
	$(CC) $(FLAGS) -O1 $(BASENAME).c -o $(BASENAME).1 
O2:
	$(CC) $(FLAGS) -O2 $(BASENAME).c -o $(BASENAME).2
O3:
	$(CC) $(FLAGS) -O3 $(BASENAME).c -o $(BASENAME).3 
Os:
	$(CC) $(FLAGS) -Os $(BASENAME).c -o $(BASENAME).small 
Ofast:
	$(CC) $(FLAGS) -Ofast $(BASENAME).c -o $(BASENAME).fast

clean:
	rm -Rf *~
