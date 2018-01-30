
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "Node.h"
#include "Heap.h"

void Node::insertNewStrings(char *value, int index)
{
    this->strings[index] = Heap::createString(value);
    this->frequencies[index] = 1;
}

void Node::insertToTemp(int index,int capacity,int i)
{
    memmove(&(indexMatrix[index + 1]), &(indexMatrix[index]), ((capacity - index) -1) * sizeof(int));
    indexMatrix[index] = i;
}

void Node::setStrings(char *string, int index)
{
    if (string != NULL)
    {
        free(this->strings[index]);
        this->strings[index] = NULL;
        this->strings[index] = Heap::createString(string);
    }
    else
    {
        free(this->strings[index]);
        this->strings[index] = NULL;
    }
}
