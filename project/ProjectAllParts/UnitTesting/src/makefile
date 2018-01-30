SRC = main.cpp trie.cpp HelperFunctions.cpp functions.cpp BloomFilter.cpp murmur3.cpp Heap.cpp Node.cpp hash.cpp
OBJS = main.o trie.o HelperFunctions.o functions.o BloomFilter.o murmur3.o Heap.o Node.o hash.o
HEADER = trie.h HelperFunctions.h functions.h BloomFilter.h murmur3.h Heap.h Node.h hash.h Jobs.h Scheduler.h UnitTestingFunctions.h
FLAGS = -c -g -Wall -lm
EXE = TESTING
CC = g++

all : Testing
	
Testing : $(OBJS)
	$(CC) $(OBJS) -o $(EXE) -lpthread

main.o : main.cpp
	$(CC) $(FLAGS) main.cpp

trie.o : trie.cpp
	$(CC) $(FLAGS) trie.cpp

HelperFunctions.o : HelperFunctions.cpp
	$(CC) $(FLAGS) HelperFunctions.cpp

functions.o : functions.cpp
	$(CC) $(FLAGS) functions.cpp

BloomFilter.o : BloomFilter.cpp
	$(CC) $(FLAGS) BloomFilter.cpp

murmur3.o : murmur3.cpp
	$(CC) $(FLAGS) murmur3.cpp

Heap.o : Heap.cpp
	$(CC) $(FLAGS) Heap.cpp

Node.o : Node.cpp
	$(CC) $(FLAGS) Node.cpp

hash.o : hash.cpp
	$(CC) $(FLAGS) hash.cpp


clean :
	rm -rf $(OBJS) $(EXE)

count :
	wc $(SRC) $(HEADER)
