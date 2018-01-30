#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <cassert>


#define HASH_SIZE 20
#define HASH_BUCKET_SIZE 10

using namespace std;

class Trie_Node;

class Hash_Bucket{
private:

public:
    int timesSplit;
    int numberOfNodes;
    bool isOverflow;
    Trie_Node *table;
    Hash_Bucket();
    ~Hash_Bucket();
    //int createTable();
};

class Info_Hash{
private:

public:
    int round;
    int tosplit;
    Hash_Bucket *buckets;
    Info_Hash();
    ~Info_Hash();
    Trie_Node* addNode( char *,int);
    Trie_Node* searchNode(char *);
    int deleteNode(char *);
    uint64_t hashFunc(char *);
    int createBucket(bool isStart);
};

