CC = gcc
FLAGS = -Wall
TARGETS = src/runelection.o src/curator.o

runelection:$(TARGETS)
	$(CC) $(FLAGS) -o runelection $(TARGETS)

runelection.o:src/runelection.c
	$(CC) $(FLAGS) -o src/runelection.o -c src/runelection.c

curator.o:src/curator.c
	$(CC) $(FLAGS) -o src/curator.o - src/curator.c

.PHONY : clean

clean:
	rm -f $(TARGETS) ru