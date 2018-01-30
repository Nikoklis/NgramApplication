#ifndef __guard9__
#define __guard9__

# include "Scheduler.h"

extern int Path_To_CompressTimesDuplicated;

//To be visible for Threads
extern ifstream queryFile;
extern bool EOF_Query_File;
extern Scheduler *Sch;

void takeWord(char **, char *);
Trie_Node *binarySearch(char *, int, Trie_Node *);
Trie_Node *binarySearch_for_Add(char *, int , Trie_Node *,int *);

void insertToMatrix(Trie_Node **&, int, Trie_Node *,int *,int);
void printN_GramMatrix(Trie_Node **&, int, int);
void cleanN_GramMatrix(Trie_Node **&, int);
void N_GramMatrix_to_String(Trie_Node **&, int , char **, bool);


void searchTrie(Index *, char *, Trie_Node **&, BloomFilter *&,Heap &);
void search_Compressed_Trie(Index *, char *, Heap &, int ,int);
void concatenate_N_Gram_Words(char *&, char *, int);
void final_In_SuperNode_to_String(char **, char *, short *&, int , int);

void printTrie(Trie_Node *);
void deleteTrie(Trie_Node *);



void Create_Jobs_and_Feed_Threads(		Index *base,
										Heap *heap);



void *Wait_for_Job_and_Execute(void *);


#endif
