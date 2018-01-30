//
// Created by nikoklis on 29/11/2017.
//

#ifndef HEAPSTRINGS_HEAP_H
#define HEAPSTRINGS_HEAP_H

#include "Node.h"

#define HEAPSIZE 5000

class Heap
{
  public:
    Heap(int capacity);

    virtual ~Heap();

    const Node &getNode() const
    {
        return node;
    }

    int getCapacity() const
    {
        return capacity;
    }

    int getCurrentCapacity() const
    {
        return currentCapacity;
    }

    void setNode(const Node &node);

    void setCapacity(int capacity)
    {
        this->capacity = capacity;
    }

    void setCurrentCapacity(int currentCapacity)
    {
        this->currentCapacity = currentCapacity;
    }

    void heapify(int index);

    void swap(int *x, int *y)
    {
        int temp = *x;
        *x = *y;
        *y = temp;
    }

    char *extractMin();

    void heapSort(int topK);

    void InsertHeap(char *value);

    int parent(int i)
    {
        return (i - 1) / 2;
    }

    int left(int i)
    {
        return (2 * i + 1);
    }

    int right(int i)
    {
        return (2 * i + 2);
    }

    char *getMin();

    static char *createString(char *value);

    void reset();

    void print(int k);

    bool isEmpty()
    {
        return Empty;
    }

  private:
    Node node;
    int capacity;
    int currentCapacity;
    bool Empty;
};

#endif //HEAPSTRINGS_HEAP_H
