//
// Created by nikoklis on 29/11/2017.
//

#ifndef HEAPSTRINGS_NODE_H
#define HEAPSTRINGS_NODE_H

#include "stdlib.h"
#include "string.h"

class Heap;

class Node
{
  public:
    Node(){};

    Node(int size)
    {
        this->frequencies = (int *)malloc(sizeof(int) * size);
        this->strings = (char **)malloc(sizeof(char *) * size);
        // this->indexMatrix = (int *)malloc(sizeof(int) * size);
        for (int i = 0; i < size; i++)
        {
            this->strings[i] = NULL;
            this->frequencies[i] = 0;
            // this->indexMatrix[i] = -1;
        }
    };

    virtual ~Node(){
        
    };

    int *getFrequencies() const
    {
        return frequencies;
    }

    char **getStrings() const
    {
        return strings;
    }

    void setFrequencies(int value, int index)
    {
        this->frequencies[index] = value;
    }

    void setStrings(char *string, int index);

    void CleanString(char *string, int index)
    {
        free(this->strings[index]);
        this->strings[index] = NULL;
    }

    void insertNewStrings(char *value, int index);

    void insertToTemp(int index, int capacity, int i);

    int *getIndexMatrix()
    {
        return indexMatrix;
    }

    bool checkIfStringExists(char *value, int n, int *index, int choice)
    {
        if (choice == 1)
        {
            int cond, low, high, mid = 0;
            low = 0;           /* Lower limit for searching */
            high = n - 1;      /* Upper limit for searching */
            int sm_eq_big = 1; //0 if equal, 1 if smaller ,2 if bigger

            while (low <= high)
            {
                /* Do we have space for searching? */

                mid = (low + high) / 2; /* Medium element of search interval */
                                        /* to compare with word we are looking for */

                if (indexMatrix[mid] != -1)
                {
                    if ((cond = strcmp(value, strings[indexMatrix[mid]])) < 0)
                    {
                        high = mid - 1; /* Not found, word might be at first half */
                        sm_eq_big = 1;
                    }
                    else if (cond > 0)
                    {
                        low = mid + 1;
                        sm_eq_big = 2;
                    }
                    else
                    {
                        *index = mid;
                        return true;
                    }
                }
                else
                {
                    high = mid - 1;
                    sm_eq_big = 1;
                }
            }
            //the position of element to be inserted

            if (sm_eq_big == 1 || sm_eq_big == 0)
                *index = mid;
            else if (sm_eq_big == 2)
                *index = mid + 1;

            return false;
        }
        else if (choice == 0)
        {
            int i = 0;
            while (this->strings[i] != NULL)
            {
                if (strcmp(this->strings[i], value) == 0)
                {
                    (*index) = i;
                    return true;
                }
                i++;
            }
            return false;
        }
        return false;
    }

    void doubleMatrixes(int lastPrev, int newCapacity)
    {
        this->strings = (char **)realloc(this->strings, sizeof(char *) * newCapacity);
        this->frequencies = (int *)realloc(this->frequencies, sizeof(int) * newCapacity);
        // this->indexMatrix = (int *)realloc(this->indexMatrix, sizeof(int) * newCapacity);
        for (int i = lastPrev; i < newCapacity; i++)
        {
            this->strings[i] = NULL;
        }
    }

    void setIndexMatrix(int value, int index)
    {
        // this->indexMatrix[index] = value;
    }

  private:
    int *frequencies;
    char **strings;
    int *indexMatrix;
};

#endif //HEAPSTRINGS_NODE_H
