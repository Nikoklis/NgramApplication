#include "trie.h"
#include "HelperFunctions.h"
#include "functions.h"
#include <unistd.h>

int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
int THREADS_NUM = 2 * num_cores;

int Path_To_CompressTimesDuplicated = 1; //For the path matrix used in compress
int countNumberOfThreadsDone = 0;
int numberOfThreadsWaiting = 0;
int numberOfJobs = 0;
bool Full_Queue = false;

//Mutex for Thread Sync on sceduler
pthread_mutex_t sch_mutex = PTHREAD_MUTEX_INITIALIZER;
//Condition Variable on Sceduler's Queue
pthread_cond_t sch_queue_empty = PTHREAD_COND_INITIALIZER;
//mutex for syncing the last working theread with main
pthread_mutex_t barrier_mutex = PTHREAD_MUTEX_INITIALIZER;
//
pthread_mutex_t numberOfThreadsWaiting_mutex = PTHREAD_MUTEX_INITIALIZER;

// pthread_mutex_t waiting_for_threads_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waiting_for_threads = PTHREAD_COND_INITIALIZER;

pthread_mutex_t heap_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t Full_Queue_mutex = PTHREAD_MUTEX_INITIALIZER;

//To be visible for Threads
ifstream queryFile;
bool EOF_Query_File = false;
Scheduler *Sch = NULL;
Parallel_Printing_Array *Th_Array = NULL;

int main(int argc, char *argv[])
{
    

    //ask for users
    // cerr << "The number of cores in your machine is : " << num_cores << endl;
    // cerr << "Please type the number of threads to be created " << endl;
    // cin >> THREADS_NUM;
    // if (THREADS_NUM < 0 || !cin)
    // {
    //     cerr << "Bad value --- Setting number of threads to default " << 2 * num_cores << endl;
    //     THREADS_NUM = 2 * num_cores;
    // }

    


    //matrix that holds the path in the trie to be compressed.
    Trie_Node **Path_To_Compress;

    Path_To_Compress = (Trie_Node **)malloc(PATH_TO_COMPRESS_SZE * sizeof(Trie_Node *));

    //initialize matrixes
    memset(Path_To_Compress, (int)NULL, PATH_TO_COMPRESS_SZE * sizeof(Trie_Node *));

    // ValidateInput(&inputFilePosition, &queryFilePosition, argc, argv);

    // if (inputFilePosition != 0 && queryFilePosition != 0)
    // {
        // Index *root = new Index();


        // ifstream f(argv[inputFilePosition]);
        // string line;

        // Heap heap(HEAPSIZE);


        //UNIT TESTING
        Index *testTrie = new Index();
        //first Tests for Add and Delete 
        testTrie->TEST_ADD_DELETE();
        ////
        cout << "Passed Add and Delete Tests" << endl;
        //second Tests 
        testTrie->TEST_BLOOMFILTER_HEAP();
        cout << "Passed Bloomfilter and Heap tests" << endl;
        //

        //third Tests
        testTrie->TEST_QUERY_VERSIONING();
        cout << "Passed Query versioning tests" << endl;
        //

        ////
        deleteTrie(testTrie->getRoot());
        delete (testTrie);
        // ///////////////

        //MAIN PROGRAMM

        // root->Init(inputFilePosition, argv, Path_To_Compress);
        // // char C[] = "january";
        // // cout<<"omg "<<root->getRoot()->rootTable->searchNode(C)->Get_If_Final()<<endl;

        // //open query file
        // queryFile.open(argv[queryFilePosition]);

        // Sch = new Scheduler();

        // //Create threads
        // pthread_t *thread_Pool = (pthread_t *)malloc(THREADS_NUM * sizeof(pthread_t));

        // // Thread_Args th_args(Sch,queryFile);

        // for (int i = 0; i < THREADS_NUM; i++)
        // {

        //     err = pthread_create(&thread_Pool[i], NULL, &Wait_for_Job_and_Execute, NULL);

        //     if (err != 0)
        //         fprintf(stderr, "%s: %s\n", "pthread_create", strerror(err));
        // }

        // //master thread lock and waits for the threads to gather in cond-var and awake him

        // pthread_mutex_lock(&barrier_mutex);

        // while (numberOfThreadsWaiting < THREADS_NUM)
        // {

        //     pthread_cond_wait(&waiting_for_threads, &barrier_mutex);
        // }

        // pthread_mutex_unlock(&barrier_mutex);

        // Create_Jobs_and_Feed_Threads(root,
        //                              &heap);

        // // root->Work(queryFilePosition, N_GramMatrix, argv, blF, heap);

        // for (int i = 0; i < THREADS_NUM; i++)

        //     err = pthread_join(thread_Pool[i], NULL);

        // if (err != 0)
        // {
        //     fprintf(stderr, "%s: %s\n", "pthread_join", strerror(err));
        //     exit(EXIT_FAILURE);
        // }

        //FREE EVERUTHING

        // deleteTrie(root->getRoot());
        // delete root;
        // delete Sch;
        // free(thread_Pool);

        // for (int i = 0; i < heap.getCurrentCapacity(); i++)
        // {
        //     if (heap.getNode().getStrings()[i] != NULL)
        //         free(heap.getNode().getStrings()[i]);
        // }

        // if (heap.getNode().getStrings() != NULL)
        //     free(heap.getNode().getStrings());

        // if (heap.getNode().getFrequencies() != NULL)
        //     free(heap.getNode().getFrequencies());
        //////

        //////
        // queryFile.close();
    // }

    free(Path_To_Compress);

    destroy_Mutexes_and_CondVars();

    //end counting the time of our programm
    // clock_me (in seconds) is : " << elapsed << endl;
    ////
}
