//
// Created by nikoklis on 29/11/2017.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "Heap.h"

using namespace std;

Heap::Heap(int capacity) : node(capacity)
{
    this->capacity = capacity;
    this->currentCapacity = 0;
    this->Empty = true;
}

Heap::~Heap()
{
}

char *Heap::createString(char *value)
{
    char *temp = (char *)malloc(strlen(value) + 1);
    strncpy(temp, value, strlen(value));
    temp[strlen(value)] = '\0';
    return temp;
}

void Heap::InsertHeap(char *value)
{
    int checkIfValueExists = 0;
    int index = 0;

    if (currentCapacity == capacity)
    {
        //we have to double our heap size
        capacity *= 2;
        node.doubleMatrixes(currentCapacity, capacity);
    }

    // First insert the new key at the end
    
    checkIfValueExists = node.checkIfStringExists(value,currentCapacity, &index,0);
    if (checkIfValueExists)
    {
        // node.checkIfStringExists(value,currentCapacity,&index,0);
        // int IndexMyVar = node.getIndexMatrix()[index];
        int tempF = 0;
        tempF = node.getFrequencies()[index] + 1;
        node.setFrequencies(tempF, index);
        // int checkIfMoved = 0;
        while (index != 0 && node.getFrequencies()[parent(index)] < node.getFrequencies()[index])
        {
            // node.checkIfStringExists(node.getStrings()[parent(IndexMyVar)],currentCapacity,&index,1);
            
            swap(&node.getFrequencies()[index], &node.getFrequencies()[parent(index)]);
            char *temp = createString(node.getStrings()[parent(index)]);

            node.setStrings(node.getStrings()[index], parent(index));
            node.setStrings(temp, index);

            // swap(&(node.getIndexMatrix()[IndexMyVar]),&(node.getIndexMatrix()[index]));

            index = parent(index);

            
            free(temp);
        }
    }
    else
    {
        if (currentCapacity == 0)
        {
            Empty = true;
        }
        else
        {
            Empty = false;
        }
        currentCapacity++;
        int i = currentCapacity - 1;
        node.insertNewStrings(value, i);
        // Fix the max heap property if it is violated
        while (i != 0 && node.getFrequencies()[parent(i)] < node.getFrequencies()[i])
        {
            swap(&node.getFrequencies()[i], &node.getFrequencies()[parent(i)]);
            //char *temp = createString(node.getStrings()[value]);
            char *temp = createString(node.getStrings()[parent(i)]);

            node.setStrings(node.getStrings()[i], parent(i));
            node.setStrings(temp, i);

            i = parent(i);
            free(temp);
        }
        // node.insertToTemp(index,capacity,i);
        
    }
}

// char *Heap::extractMin() {

//     if (currentCapacity == 1)
//     {
//         currentCapacity--;
//         return node.getStrings()[0];
//     }

//     // Store the minimum value, and remove it from heap
//     char *root = createString(node.getStrings()[0]);

//     node.setStrings(node.getStrings()[0],currentCapacity-1);
//     currentCapacity--;
//     MinHeapify(0);

//     return root;
// }

// main function to do heap sort
// void Heap::heapSort(int topK)
// {
//     int *matrix = node.getFrequencies();
//     // // Build heap (rearrange array)
//     for (int i = currentCapacity / 2 - 1; i >= 0; i--)
//         heapify(i);

//     // One by one extract an element from heap
//     for (int i = currentCapacity - 1; i >= 0; i--)
//     {
//         // Move current root to end
//         swap(&matrix[0], &matrix[i]);
//         // Store the minimum value, and remove it from heap
//         char *temp = createString(node.getStrings()[0]);

//         node.setStrings(node.getStrings()[i], 0);
//         node.setStrings(temp, i);
//         // currentCapacity--;

//         // call max heapify on the reduced heap
//         heapify(i);
//     }

//     //Here we check for same frequency elements and we sort
//     //them with strings
//     int countSameElements = 0;
//     int lastSame = 0;
//     for (int i = 0; i < topK; i = countSameElements)
//     {
//         // w = lastSame;
//         if (countSameElements >= topK)
//             break;
//         lastSame = countSameElements;

//         if (countSameElements >= topK)
//         {
//             return;
//         }

//         if (matrix[countSameElements] == matrix[countSameElements + 1])
//         {
//             for (int l = countSameElements; matrix[l] == matrix[l + 1]; l++)
//             {
//                 countSameElements++;
//             }
//         }
//         if (countSameElements == lastSame)
//         {
//             countSameElements++;
//             continue;
//         }

//         int max = -1;
//         for (int l = i; l < countSameElements + 1; l++)
//         {
//             max = l;
//             for (int j = l; j < countSameElements + 1; j++)
//             {
//                 if (strcmp(node.getStrings()[j], node.getStrings()[max]) < 0)
//                 {
//                     max = j;
//                 }
//             }

//             if (max != -1)
//             {
//                 //swap the elements of max and l
//                 swap(&matrix[l], &matrix[max]);
//                 char *temp = Heap::createString(node.getStrings()[l]);

//                 node.setStrings(node.getStrings()[max], l);
//                 node.setStrings(temp, max);
//                 free(temp);
//             }
//         }
//     }
// }

void Heap::heapify(int index)
{

    int leftChild = left(index);
    int rightChild = right(index);
    int largest = index;
    if (leftChild < currentCapacity && node.getFrequencies()[leftChild] > node.getFrequencies()[largest])
        largest = leftChild;
    if (rightChild < currentCapacity && node.getFrequencies()[rightChild] > node.getFrequencies()[largest])
        largest = rightChild;
    if (largest != index)
    {
        swap(&node.getFrequencies()[index], &node.getFrequencies()[largest]);

        char *temp = createString(node.getStrings()[largest]);

        node.setStrings(node.getStrings()[index], largest);
        node.setStrings(temp, index);

        heapify(largest);
    }
}

char *Heap::getMin()
{
    return node.getStrings()[0];
}

void Heap::print(int k)
{
    cout << "Top: ";
    for (int i = 0; i < k; i++)
    {

        cout << node.getStrings()[i]; //<< node.getFrequencies()[i];
        if (i == k - 1)
            cout << endl;
        else
            cout << "|";
        
    }
}

void Heap::reset()
{
    for (int i = 0; i < currentCapacity; i++)
    {
        if (node.getStrings()[i] != NULL)
        {
            // char *temp = node.getStrings()[i];
            //free(temp);
            // node.setStrings(NULL,i);
            node.CleanString(NULL, i);
            node.setFrequencies(0, i);
            node.setIndexMatrix(-1,i);
        }
    }
    currentCapacity = 0;
    Empty = true;
}
