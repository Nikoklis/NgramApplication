#ifndef __guard2__
#define __guard2__

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <istream>
#include <sstream>
#include <stdio.h>

#include "Heap.h"

//Mutex for Thread Sync on sceduler
extern pthread_mutex_t sch_mutex;
//Condition Variable on Sceduler's Queue
extern pthread_cond_t sch_queue_empty;
//mutex for syncing the last working theread with main
extern pthread_mutex_t barrier_mutex;

extern pthread_mutex_t numberOfThreadsWaiting_mutex;
extern pthread_cond_t waiting_for_threads;

extern pthread_mutex_t heap_mutex;

extern pthread_mutex_t Full_Queue_mutex;

using namespace std;

void ValidateInput(int *inputFilePosition, int *queryFilePosition, int argc, char **argv);
void GetLineInfo(string line, int &countWords, char **&words);
// void RecursiveSorting(Trie_Node *current);
void convertStringToChar(string line, char *&str);
void FreeInfo(char **&, int);
// int checkAddFunction(Trie_Node *root,char *,int , char**,int);
// int checkDeleteFunciton(Trie_Node *root,char *newText,int countWords,char **words);


void quicksort_body(int size, int *&frequencies, char **&strings,int currentCapacity);
void quickSortRecursive(int *&matrix,char **&strings,int up,int down);

void destroy_Mutexes_and_CondVars(void);

#endif