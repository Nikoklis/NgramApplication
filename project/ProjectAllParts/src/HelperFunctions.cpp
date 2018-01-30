#include "HelperFunctions.h"

void convertStringToChar(string line, char *&str)
{
    //    char *space;

    line.copy(str, line.length());
    str[line.length()] = '\0';
}

void ValidateInput(int *inputFilePosition, int *queryFilePosition, int argc, char **argv)
{
    if (argc != 5)
    {
        cout << "We require more elements from the user --- exiting" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        if (strcmp(argv[1], "-i") == 0 &&
            strcmp(argv[2], "-q") != 0 &&
            argv[2] != NULL)
        {
            (*inputFilePosition) = 2;
        }
        else if (strcmp(argv[1], "-q") == 0 &&
                 strcmp(argv[2], "-i") != 0 &&
                 argv[2] != NULL)
        {
            (*queryFilePosition) = 2;
        }

        if (strcmp(argv[3], "-i") == 0 &&
            strcmp(argv[4], "-q") != 0 &&
            argv[4] != NULL)
        {
            (*inputFilePosition) = 4;
        }
        else if (strcmp(argv[3], "-q") == 0 &&
                 strcmp(argv[4], "-i") != 0 &&
                 argv[4] != NULL)
        {
            (*queryFilePosition) = 4;
        }
    }
}

void GetLineInfo(string line, int &countWords, char **&words)
{
    istringstream ss(line);
    string word;
    int i = 0;

    while (getline(ss, word, ' '))
    {
        countWords++;
    }
    // cout << countWords << endl;
    // cout << countWords << word << endl;
    words = (char **)malloc(sizeof(char *) * countWords); //new char *[countWords];
    if (words == NULL)
    {
        cout << "Malloc error" << endl;
        exit(EXIT_FAILURE);
    }
    i = 0;
    istringstream ss1(line);
    while (getline(ss1, word, ' '))
    {
        words[i] = (char *)malloc(word.length() + 1);
        word.copy(words[i], word.length());
        words[i][word.length()] = '\0';
        i++;
    }
}

void FreeInfo(char **&words, int countWords)
{
    for (int i = 0; i < countWords; i++)
    {
        free(words[i]);
        words[i] = NULL;
    }
    free(words);
    words = NULL;
}

// void InsertSort(int n, int *matrix, Heap &heap)
// {

//     //FIRST sort as far as frequencies are concerned
//     //SECOND sort elements with the same frequencies as far as their string
//     //values are concerned

//     //use insertSort to sort matrix
//     //frequency based first
//     int i, j;
//     Node node = heap.getNode();
//     int tempN = n;
//     n = heap.getCurrentCapacity();
//     for (i = 1; i < n; i++)
//     {
//         j = i - 1;
//         while (j >= 0 && (matrix[j] < matrix[j + 1]))
//         {
//             int tempF;
//             tempF = matrix[j];
//             matrix[j] = matrix[j + 1];
//             matrix[j + 1] = tempF;

//             char *temp = Heap::createString(node.getStrings()[j + 1]);
//             char *swap1 = heap.getNode().getStrings()[j];
//             node.setStrings(swap1, j + 1);
//             node.setStrings(temp, j);

//             j--;
//             free(temp);
//         }
//     }

//     //Here we check for same frequency elements and we sort
//     //them with strings
//     int countSameElements = 0;
//     int lastSame = 0;
//     // for (int i =0 ; i < tempN; i = countSameElements){

//     //     if (countSameElements >= tempN)
//     //         break;
//     //     lastSame = countSameElements;

//     //     if (countSameElements >= tempN)
//     //     {
//     //         return;
//     //     }

//     //     if (matrix[i] == matrix[i+1])
//     //     {
//     //         for (int l = countSameElements; matrix[l] == matrix[l + 1]; l++)
//     //         {
//     //             countSameElements++;
//     //         }
//     //     }
//     //      else
//     //     {
//     //         for (int l = nextDifferent; matrix[l] == matrix[l + 1]; l++)
//     //         {
//     //             nextDifferent++;
//     //         }
//     //     }

//     //     if (countSameElements == lastSame)
//     //     {
//     //         countSameElements += nextDifferent;
//     //         continue;
//     //     }

//     //     cout <<countDifferent << endl;
//     // }

//     for (int i = 0; i < tempN; i = countSameElements)
//     {
//         // w = lastSame;

//         if (countSameElements >= tempN)
//             break;
//         lastSame = countSameElements;

//         if (countSameElements >= tempN)
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
//         // else
//         // {
//         //     // for (int l = nextDifferent; matrix[l] == matrix[l + 1]; l++)
//         //     // {
//         //     //     nextDifferent++;
//         //     // }
//         // }

//         if (countSameElements == lastSame)
//         {
//             countSameElements++;
//             continue;
//         }

//         int max = -1;
//         for (int l = i; l < countSameElements + 1; l++)
//         {
//             max = l;
//             for (j = l; j < countSameElements + 1; j++)
//             {
//                 if (strcmp(node.getStrings()[j], node.getStrings()[max]) < 0)
//                 {
//                     max = j;
//                 }
//             }

//             if (max != -1)
//             {
//                 //swap the elements of max and l
//                 swap(matrix[l], matrix[max]);
//                 char *temp = Heap::createString(node.getStrings()[l]);

//                 node.setStrings(node.getStrings()[max], l);
//                 node.setStrings(temp, max);
//                 free(temp);
//             }
//         }
//     }
//     //////////
// }

void quicksort_body(int size, int *&frequencies, char **&strings, int currentCapacity)
{

    //quicksort the matrix as far as their frequencies are concerned
    quickSortRecursive(frequencies, strings, 0, currentCapacity - 1);

    //Here we check for same frequency elements and we sort
    //them with strings

    //Selection sort here
    //used because matrix is small and easy to implement
    //NOTE::can be changed if we create a new QuickSort function for strings
    //TO DO in latter parts ---NOTE : not changed now because
    //the change in time is too small to notice between the 2 methods
    int countSameElements = 0;
    int lastSame = 0;
    int j = 0;
    for (int i = 0; i < size; i = countSameElements)
    {
        // w = lastSame;
        if (countSameElements >= size)
            break;
        lastSame = countSameElements;

        if (countSameElements >= size)
        {
            return;
        }

        if (frequencies[countSameElements] == frequencies[countSameElements + 1])
        {
            for (int l = countSameElements; frequencies[l] == frequencies[l + 1]; l++)
            {
                countSameElements++;
            }
        }
        if (countSameElements == lastSame)
        {
            countSameElements++;
            continue;
        }

        int max = -1;
        for (int l = i; l < countSameElements + 1; l++)
        {
            max = l;
            for (j = l; j < countSameElements + 1; j++)
            {
                if (strcmp(strings[j], strings[max]) < 0)
                {
                    max = j;
                }
            }

            if (max != l)
            {
                //swap the elements of max and l

                swap(frequencies[l], frequencies[max]);
                char *temp = Heap::createString(strings[l]);

                // cout << "to free us all " << strlen(strings[l]) << " " << strings[l] << endl;
                free(strings[l]);
                // strings[l] = NULL;
                strings[l] = Heap::createString(strings[max]);

                free(strings[max]);
                strings[max] = temp;
            }
        }
    }
}

void quickSortRecursive(int *&frequencies, char **&strings, int up, int down)
{
    int start, end;
    start = up;
    /* Save start position of small elements */
    end = down;
    /* Save end position of large elements */
    while (up < down)
    {
        /* Pivot element is at up position */
        while (frequencies[down] <= frequencies[up] && up < down)
            /* Let down elements */
            down--;
        /* larger than pivot stay where they are */
        if (up != down)
        {
            /* If pivot is not reached */
            swap(frequencies[up], frequencies[down]); /* echange it with smaller element */
            char *temp = Heap::createString(strings[up]);

            free(strings[up]);
            strings[up] = NULL;
            strings[up] = Heap::createString(strings[down]);

            free(strings[down]);
            strings[down] = NULL;
            strings[down] = temp;

            (up++); /* Pivot is at down position, move up a bit further */
        }
        while (frequencies[up] >= frequencies[down] && up < down)
            /* Let up elements */
            up++;
        /* smaller than pivot stay where they are */
        if (up != down)
        {
            /* If pivot is not reached */
            swap(frequencies[up], frequencies[down]); /* exchange it with larger element */
            char *temp = Heap::createString(strings[up]);

            free(strings[up]);
            strings[up] = NULL;
            strings[up] = Heap::createString(strings[down]);

            free(strings[down]);
            strings[down] = NULL;
            strings[down] = temp;

            down--; /* Pivot is at up position, move down a bit further */
        }
    }
    /* Now up = down is the position of the pivot element */
    if (start < up - 1)                                          /* Is there at least one element left of pivot? */
        quickSortRecursive(frequencies, strings, start, up - 1); /* Quick(sort) smaller elements */
    if (end > down + 1)                                          /* Is there at least one element right of pivot? */
        quickSortRecursive(frequencies, strings, down + 1, end); /* Quick(sort) larger elements */
}


void destroy_Mutexes_and_CondVars(){

    int err = 0;

    err = pthread_mutex_destroy(&sch_mutex);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "ppthread_mutex_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }

    err = pthread_mutex_destroy(&barrier_mutex);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "ppthread_mutex_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }

    err = pthread_mutex_destroy(&numberOfThreadsWaiting_mutex);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "ppthread_mutex_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }

    err = pthread_mutex_destroy(&heap_mutex);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "ppthread_mutex_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }

    err = pthread_mutex_destroy(&Full_Queue_mutex);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "ppthread_mutex_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }

    err = pthread_cond_destroy(&sch_queue_empty);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "thread_cond_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }

    err = pthread_cond_destroy(&waiting_for_threads);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", "thread_cond_destroy", strerror(err));
        exit(EXIT_FAILURE);
    }
}

