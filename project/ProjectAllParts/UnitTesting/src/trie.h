#ifndef __guard1__
#define __guard1__

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <cassert>
#include <pthread.h>

#include "BloomFilter.h"
#include "Heap.h"
#include "hash.h"

// extern int N_GramMatrixTimesDuplicated; //initiliazed in main.cpp
// extern int Path_To_CompressTimesDuplicated;

class Parallel_Printing_Array;
using namespace std;

#define CHILDREN_MATRIX 5

#define N_GRAM_MATRIX_SZE 1000 //matrix that keeps N-Grams to be printed
#define PATH_TO_COMPRESS_SZE 1000

#define SMALL_WORD_SIZE 25

extern int THREADS_NUM;


extern Parallel_Printing_Array *Th_Array;
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

extern int countNumberOfThreadsDone;
extern int numberOfJobs;
extern bool Full_Queue;

class Trie_Node
{
private:
  //Matrix to store small words so we can have locality
  char word[SMALL_WORD_SIZE];

  //Failsafe if we get a big word and also used as the word in the compressed Trie
  char *bigWord;

  Trie_Node *children;
  bool isFinal;

  //Helping for print
  bool isPrinted;

  int timesDuplicated;
  int numberOfChildren;

  //0---not initialized trie node, 1----small word , 2---- big word
  unsigned short state;

  //Used only if the Trie is compressed.

  //An array of shorts keeping the length of each word of the node.
  //If the short is positive the word is final.
  short *each_Word_Size_Array;

  //is the size of the above array
  int numbeOfWordsInSuperNode;

  //if the node is to be compressed if we convert the Trie to Static
  bool toBeCompressed;

  //For Dynamic
  int Added;
  int Deleted;

public:
  Info_Hash *rootTable;
  Trie_Node();
  ~Trie_Node();

  Trie_Node(char *newWord);
  Trie_Node(bool);
  void operator=(const Trie_Node &newObject);

  void Add(int, char **, int,int);
  void createChildren(Trie_Node created);

  int Remove(char **, int,int);
  void clearSelf();
  int RemoveFinal();

  Trie_Node *getChildren();
  int getNumberOfChildren();
  int getTimesDuplicated();
  char *getWord();
  bool Get_If_Printed();
  bool Get_If_Final();
  unsigned short Get_State();
  char *Get_Big_Word();
  short *Get_each_Word_Size_Array();
  int Get_Number_of_Words_in_SuperNode();
  int getAdded(){
    return Added;
  }

  int getDeleted(){
    return Deleted;
  }

  void Print();

  void Set_Printed(bool);
  void Set_Final(bool);
  void Set_State(unsigned short);
  void Set_Big_Word(char *);
  void setAdded(int a){
    this->Added = a;
  }

  void Compress_Trie(Trie_Node **&, int);
  void Compress_Path_and_Insert_To_SuperNode(Trie_Node **&, int, int, Trie_Node *&);
  void print_Compressed_Trie();

  void Add_To_Compression_Path(Trie_Node **&, int, Trie_Node *&);
  void Clean_Compression_Path(Trie_Node **&, int);
};

class Index
{
private:
  Trie_Node *root;

public:
  Index();
  ~Index()
  {
    delete root; 
  }

  Trie_Node *getRoot();

  void Init(int inputFilePosition, char **argv, Trie_Node **&Path_To_Compress);

  void TEST_ADD_DELETE();
  void TEST_BLOOMFILTER_HEAP();
  void TEST_QUERY_VERSIONING();
};

#endif
