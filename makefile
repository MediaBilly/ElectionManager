CC = gcc
FLAGS = -Wall
TARGETS = src/runelection.o src/curator.o src/voter.o src/redblacktree.o src/utilities.o src/bloomfilter.o

runelection:$(TARGETS)
	$(CC) $(FLAGS) -o runelection $(TARGETS)

src/runelection.o:src/runelection.c
	$(CC) $(FLAGS) -o src/runelection.o -c src/runelection.c

src/curator.o:src/curator.c
	$(CC) $(FLAGS) -o src/curator.o -c src/curator.c

src/voter.o:src/voter.c
	$(CC) $(FLAGS) -o src/voter.o -c src/voter.c

src/redblacktree.o:src/redblacktree.c
	$(CC) $(FLAGS) -o src/redblacktree.o -c src/redblacktree.c

src/utilities.o:src/utilities.c
	$(CC) $(FLAGS) -o src/utilities.o -c src/utilities.c

src/bloomfilter.o:src/bloomfilter.c
	$(CC) $(FLAGS) -o src/bloomfilter.o -c src/bloomfilter.c

.PHONY : clean

clean:
	rm -f $(TARGETS) runelection
