#include "trie.h"
#include "functions.h"
#include "HelperFunctions.h"

Index::Index()
{

    root = new Trie_Node(true);
    if (root == NULL)
    {
        cout << "Error in creating root Node" << endl;
        exit(EXIT_FAILURE);
    }
}

Trie_Node *Index::getRoot()
{
    return this->root;
}
void Testing_Add_In_Root(Trie_Node *root)
{
    ifstream init;
    init.open("../UnitTestFiles/testingAddInRoot.init");
    ifstream work;
    string line;
    int countWords = 0;
    char **words = NULL;
    int choice = 0;
    if (init.good())
    {
        while (getline(init, line))
        {
            GetLineInfo(line, countWords, words);
            root->Add(countWords, words, choice, 0);
            FreeInfo(words, countWords);
            countWords = 0;
        }
        int sumOfNodes = 0;
        for (int i = 0; i < HASH_SIZE; i++)
        {
            sumOfNodes += root->rootTable->buckets[i].numberOfNodes;
        }
        assert(sumOfNodes == 5);
    }
    init.close();
}

void Testing_If_Hash_Increases_HashBucket(Trie_Node *root)
{
    ifstream init;
    init.open("../UnitTestFiles/testingDoublingHashBucket.init");
    string line;
    int countWords = 0;
    char **words = NULL;
    int choice = 0;
    if (init.good())
    {
        while (getline(init, line))
        {
            GetLineInfo(line, countWords, words);
            root->Add(countWords, words, choice, 0);
            FreeInfo(words, countWords);
            countWords = 0;
        }

        //check if hash table has 1 more bucket
        int numberOfBuckets = pow(2, root->rootTable->round) + root->rootTable->tosplit;
        assert(numberOfBuckets == 2);
        ////
    }

    init.close();
}

void Testing_Added_And_Deleted_Time_In_Init(Trie_Node *root)
{
    //we are not in the first layer of the trie
    if (root->rootTable == NULL)
    {
        if (root != NULL && root->getNumberOfChildren() > 0)
        {

            Trie_Node *current = root->getChildren();
            int numberOfChildren = root->getNumberOfChildren();
            for (int i = 0; i < numberOfChildren; i++)
            {

                if (current->getChildren() != NULL)
                    Testing_Added_And_Deleted_Time_In_Init(current);
                current++;
            }

            if (root->getChildren() != NULL && numberOfChildren > 0)
            {
                Trie_Node *temp = root->getChildren();

                for (int i = 0; i < numberOfChildren; i++)
                {
                    //check for added time not in root
                    assert(temp[i].getAdded() == 0);
                    //check for deleted time
                    assert(temp[i].getDeleted() == -1);
                }
            }
        }
    }
    else
    {
        int y = pow(2, root->rootTable->round);
        int countToDelete = (HASH_SIZE * y) + root->rootTable->tosplit;
        for (int i = 0; i < countToDelete; i++)
        {
            Hash_Bucket *current = (&root->rootTable->buckets[i]);

            for (int i = 0; i < (current->numberOfNodes); i++)
            {
                if (current->table[i].Get_State() != 0)
                    Testing_Added_And_Deleted_Time_In_Init((&current->table[i]));
            }

            for (int i = 0; i < (current->numberOfNodes); i++)
            {
                //check for added time in root
                assert(current->table[i].getAdded() == 0);
                //check for deleted time
                assert(current->table[i].getDeleted() == -1);
            }
        }
    }
}

void Testing_Sorting(Trie_Node *root)
{
    char stringToTestAdd[] = "atropos is man";
    char *space = stringToTestAdd;
    char *next = NULL;
    char *firstWord = NULL;
    next = stringToTestAdd;

    int mid = 0;
    Hash_Bucket *temp = &(root->rootTable->buckets[0]);

    char buffer[] = "this";
    Trie_Node *tempCurr = binarySearch_for_Add(buffer, temp->numberOfNodes, temp->table, &mid);

    firstWord = NULL;
    takeWord(&firstWord, stringToTestAdd);
    for (int i = 0; i < tempCurr->getNumberOfChildren(); i++)
    {
        assert(strcmp(firstWord, tempCurr->getChildren()[i].getWord()) == 0);
        space = strchr(space, ' ');

        if (space != NULL)
        {
            free(firstWord);
            firstWord = NULL;

            space++;
            next = space;
            takeWord(&firstWord, space);

            if (firstWord == NULL)
            {
                firstWord = next;
            }
        }
    }
}

void Testing_HashBucket_Sorting(Trie_Node *root)
{
    Hash_Bucket *temp = (&root->rootTable->buckets[0]);
    for (int i = 0; i < temp->numberOfNodes; i++)
    {
        if (i <= temp->numberOfNodes - 1)
            if (temp->table[i + 1].Get_State() != 0)
                assert(strcmp(temp->table[i + 1].getWord(), temp->table[i].getWord()) > 0);
    }
}

void Testing_Add_InWork_With_Versioning(Trie_Node *root)
{
    ifstream work;
    work.open("../UnitTestFiles/testingAddAndDeleteInWorkVersioning.work");

    string line;
    int countWords = 0;
    char **words = NULL;
    int choice = 1;
    int addCounter = 0;

    char Michael[] = "Michael";

    if (work.good())
    {
        while (getline(work, line))
        {
            addCounter++;
            GetLineInfo(line, countWords, words);
            if (words[0][0] == 'A')
                root->Add(countWords, words, choice, addCounter);
            FreeInfo(words, countWords);
            countWords = 0;
        }
    }
    uint64_t h = root->rootTable->hashFunc(Michael);

    int mysup = pow(2, root->rootTable->round);
    int mysup2 = pow(2, root->rootTable->round + 1);

    int h1 = mysup * HASH_SIZE;
    int h2 = mysup2 * HASH_SIZE;

    int hashValue1 = h % h1;
    int hashValue2 = h % h2;
    Hash_Bucket *temp = NULL;

    if (hashValue2 > (h1 + root->rootTable->tosplit - 1))
    {
        temp = &(root->rootTable->buckets[hashValue1]);
    }
    else
    {
        temp = &(root->rootTable->buckets[hashValue2]);
    }

    int mid = 0;
    Trie_Node *tempCurr = binarySearch_for_Add(Michael, temp->numberOfNodes, temp->table, &mid);
    assert(tempCurr->getAdded() == 3);

    work.close();
}

void Testing_Delete_With_Versioning(Trie_Node *root)
{
    ifstream work;
    work.open("../UnitTestFiles/testingAddAndDeleteInWorkVersioning.work");
    string line;
    int countWords = 0;
    char **words = NULL;
    int qc = 0;

    char Michael[] = "Michael";

    if (work.good())
    {
        while (getline(work, line))
        {
            qc++;
            GetLineInfo(line, countWords, words);
            if (words[0][0] == 'D')
                root->Remove(words, countWords, qc);
            FreeInfo(words, countWords);
            countWords = 0;
        }
    }
    uint64_t h = root->rootTable->hashFunc(Michael);

    int mysup = pow(2, root->rootTable->round);
    int mysup2 = pow(2, root->rootTable->round + 1);

    int h1 = mysup * HASH_SIZE;
    int h2 = mysup2 * HASH_SIZE;

    int hashValue1 = h % h1;
    int hashValue2 = h % h2;
    Hash_Bucket *temp = NULL;

    if (hashValue2 > (h1 + root->rootTable->tosplit - 1))
    {
        temp = &(root->rootTable->buckets[hashValue1]);
    }
    else
    {
        temp = &(root->rootTable->buckets[hashValue2]);
    }

    int mid = 0;
    Trie_Node *tempCurr = binarySearch_for_Add(Michael, temp->numberOfNodes, temp->table, &mid);
    assert(tempCurr->getDeleted() == 4);
    work.close();
}

//TESTS ADD AND REMOVE TEST CASES
//HASH AND REGULAR NODES
void Index::TEST_ADD_DELETE()
{
    Trie_Node *root = this->getRoot();

    ////--------------------TESTING IF HASH ADD WORKS CORRECTLY--------------------//
    Testing_Add_In_Root(root);
    cout << "TESTING IF HASH ADD WORKS CORRECTLY ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF SORTING IN CHILDREN MATRIX(WHEN NOT IN ROOT) WORKS--------------------//
    Testing_Sorting(root);
    cout << "TESTING IF SORTING IN CHILDREN MATRIX(WHEN NOT IN ROOT) WORKS ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF HASHTABLE INCREASES--------------------//
    Testing_If_Hash_Increases_HashBucket(root);
    cout << "TESTING IF HASHTABLE INCREASES ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF ADDED NODES HAVE CORRECT INSERT TIME(INIT)--------------------//
    Testing_Added_And_Deleted_Time_In_Init(root);
    cout << "TESTING IF ADDED NODES HAVE CORRECT INSERT TIME(INIT) ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF SORTING IN HASHTABLE BUCKET WORKS--------------------//
    Testing_HashBucket_Sorting(root);
    cout << "TESTING IF SORTING IN HASHTABLE BUCKET WORKS ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF ADD WORKS WITH VERSIONING--------------------//
    Testing_Add_InWork_With_Versioning(root);
    cout << "TESTING IF ADD WORKS WITH VERSIONING ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF DELETE WORKS WITH VERSIONING--------------------//
    Testing_Delete_With_Versioning(root);
    cout << "TESTING IF DELETE WORKS WITH VERSIONING ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////////////////////////////
}

void Testing_Bloomfilter()
{
    BloomFilter *blF;
    int p = 0.001;
    int endianCheck = 1;
    // little endian if true
    if (*(char *)&endianCheck == 1)
        blF = new BloomFilter(false, p);
    else
        //big endian
        blF = new BloomFilter(true, p);

    char testQueryInserted[] = "Mpampis";
    char testQueryMissing[] = "Mpampas";

    assert((blF->is_NGram_to_BloomFilter(testQueryInserted) == false));

    blF->insert_NGram_to_BloomFilter(testQueryInserted);

    assert(blF->is_NGram_to_BloomFilter(testQueryMissing) == false);

    assert(blF->is_NGram_to_BloomFilter(testQueryInserted) == true);
}

void Testing_Add_InHeap()
{
    Heap heap(HEAPSIZE);
    int index = 0;
    char temp[] = "Mpampis";
    char temp2[] = "Dennis";

    //check if Mpampis is not in the Heap and we are going to insert a new node
    assert((heap.getNode().checkIfStringExists(temp, heap.getCurrentCapacity(), &index, 0)) == false);
    heap.InsertHeap(temp);

    //check if we have inserted succesfully a value that doesnt exist in the heap
    assert((heap.getNode().checkIfStringExists(temp, heap.getCurrentCapacity(), &index, 0)) == true);
    assert(heap.getCurrentCapacity() == 1);
    assert(heap.getNode().getFrequencies()[0] == 1);

    //check if we need to insert an already existing node
    //if the frequency increases correctly
    heap.InsertHeap(temp);
    assert(heap.getCurrentCapacity() == 1);
    assert(heap.getNode().getFrequencies()[0] == 2);

    //check if dennis doesnt exist and we are going to add a new node
    assert((heap.getNode().checkIfStringExists(temp2, heap.getCurrentCapacity(), &index, 0)) == false);

    //check if Heap property works
    heap.InsertHeap(temp2);
    heap.InsertHeap(temp2);
    heap.InsertHeap(temp2);

    //check if dennis is inserted in the heap
    assert((heap.getNode().checkIfStringExists(temp2, heap.getCurrentCapacity(), &index, 0)) == true);

    //check if we have 2 nodes in heap
    assert(heap.getCurrentCapacity() == 2);
    //check if we have not violated the heap property
    assert(heap.getNode().getFrequencies()[0] == 3);
    assert(strcmp(heap.getNode().getStrings()[0], temp2) == 0);
}

void Testing_Simple_Query(Index *b)
{
    ifstream work;
    work.open("../UnitTestFiles/testingSimpleQuery.work");

    string line;
    char *newText = NULL;
    int query_id = 0;
    Heap heap(HEAPSIZE);
    Th_Array = new Parallel_Printing_Array(3);
    if (work.good())
    {
        while (getline(work, line))
        {
            char *text = NULL;
            text = (char *)malloc(line.length() + 1);
            convertStringToChar(line, text);
            newText = text + 2; //pure text
            query_id++;

            search_Compressed_Trie(b, newText, heap, query_id, query_id);
        }
    }

    assert(strcmp(Th_Array->Matrix[0], "this is a dog|dog|cat\n") == 0);
    assert(strcmp(Th_Array->Matrix[1], "cat|this atropos\n") == 0);
    assert(strcmp(Th_Array->Matrix[2], "-1\n") == 0);
    Th_Array->Clear();

    delete Th_Array;
}

void Testing_Query_Versioning(Index *b)
{
    ifstream work;
    work.open("../UnitTestFiles/testingQueryVersioningDynamic.work");

    string line;
    char *newText = NULL;
    int query_id = 0;
    Heap heap(HEAPSIZE);
    char **words = NULL;
    int countWords = 0;
    int queryMain = 0;

    Th_Array = new Parallel_Printing_Array(4);
    if (work.good())
    {
        while (getline(work, line))
        {
            GetLineInfo(line, countWords, words);
            query_id++;
            if (words[0][0] == 'A')
            {
                b->getRoot()->Add(countWords, words, 1, query_id);
            }
            else if (words[0][0] =='D')
            {
                b->getRoot()->Remove(words,countWords,query_id);
            }
            countWords = 0;
            FreeInfo(words, countWords);
        }
        work.close();
        work.open("../UnitTestFiles/testingQueryVersioningDynamic.work");
        query_id=0;
        while (getline(work, line))
        {
            char *text = NULL;
            text = (char *)malloc(line.length() + 1);
            convertStringToChar(line, text);
            newText = text + 2; //pure text
            
            query_id ++;
            GetLineInfo(line, countWords, words);
            if (words[0][0] == 'Q')
            {
                queryMain++;
                search_Compressed_Trie(b, newText, heap, query_id, queryMain);
            }
        }
        work.close();
        assert(strcmp(Th_Array->Matrix[0], "-1\n") == 0);
        assert(strcmp(Th_Array->Matrix[1], "Jol\n") == 0);
        assert(strcmp(Th_Array->Matrix[2],"Nikos\n") == 0);
        assert(strcmp(Th_Array->Matrix[3],"-1\n") == 0);
    }

    Th_Array->Clear();
    delete Th_Array;
}
//TESTS FOR BLOOMFILTER AND HEAP
void Index::TEST_BLOOMFILTER_HEAP()
{
    ////--------------------TESTING IF BLOOMFILTER WORKS--------------------//
    Testing_Bloomfilter();
    cout << "TESTING BLOOMFILTER ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF ADD IN HEAP WORKS CORRETLY(MANY CASES WITHIN)--------------------//
    Testing_Add_InHeap();
    cout << "TESTING BLOOMFILTER ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//
}

void Index::TEST_QUERY_VERSIONING()
{
    ////--------------------TESTING IF SIMPLE QUERY WORKS(CHECKS ONLY FROM INIT FILES)--------------------//
    Testing_Simple_Query(this);
    cout << "TESTING SIMPLE QUERY ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//

    ////--------------------TESTING IF QUERY VERSIONING WORKS(ALL CASES)--------------------//
    Testing_Query_Versioning(this);
    cout << "TESTING QUERY VERSIONING ------PASSED------" << endl;
    //--------------------//--------------------//--------------------//
}

void Index::Init(int inputFilePosition,
                 char **argv,
                 Trie_Node **&Path_To_Compress)
{

    Trie_Node *root = this->getRoot();
    ifstream inputFile;
    inputFile.open(argv[inputFilePosition]);

    string line;
    string firstLine;

    int countWords = 0;
    char **words = NULL;
    int choice = 0;

    //INITIALIZATION PROCESS
    if (inputFile.good())
    {
        if (!getline(inputFile, firstLine))
        {

            cout << "Could not read first line of INIT FILE" << endl;
            exit(2);
        }

        while (getline(inputFile, line))
        {
            GetLineInfo(line, countWords, words);

            //0 cause its init time
            root->Add(countWords, words, choice, 0);
            FreeInfo(words, countWords);
            countWords = 0;
        }
        if (firstLine.compare("STATIC") == 0)
            root->Compress_Trie(Path_To_Compress, PATH_TO_COMPRESS_SZE * pow(2, Path_To_CompressTimesDuplicated - 1));
    }

    inputFile.close();
}

Trie_Node::Trie_Node()
{
    for (int i = 0; i < SMALL_WORD_SIZE; i++)
        word[i] = '\0';
    children = NULL;
    isFinal = false;
    isPrinted = false;
    timesDuplicated = 0;
    numberOfChildren = 0;
    state = 0;
    bigWord = NULL;

    each_Word_Size_Array = NULL;
    numbeOfWordsInSuperNode = 0;
    toBeCompressed = false;
    rootTable = NULL;

    Added = -1;
    Deleted = -1;
}

Trie_Node::Trie_Node(char *newWord)
{
    if (strlen(newWord) < SMALL_WORD_SIZE)
    {
        for (int i = 0; i < SMALL_WORD_SIZE; i++)
            word[i] = '\0';

        for (unsigned int i = 0; i < strlen(newWord); i++)
        {
            word[i] = newWord[i];
        }
        state = 1;
        bigWord = NULL;
    }
    else
    {
        state = 2;
        bigWord = (char *)malloc(strlen(newWord) + 1);
        strncpy(bigWord, newWord, strlen(newWord));
        bigWord[strlen(newWord)] = '\0';
    }
    children = NULL;
    isFinal = false;
    isPrinted = false;
    timesDuplicated = 0;
    numberOfChildren = 0;

    each_Word_Size_Array = NULL;
    numbeOfWordsInSuperNode = 0;
    toBeCompressed = false;
    rootTable = NULL;

    Added = -1;
    Deleted = -1;
}

Trie_Node::Trie_Node(bool isRoot)
{
    for (int i = 0; i < SMALL_WORD_SIZE; i++)
        word[i] = '\0';
    children = NULL;
    isFinal = false;
    isPrinted = false;
    timesDuplicated = 0;
    numberOfChildren = 0;
    state = 0;
    bigWord = NULL;
    each_Word_Size_Array = NULL;

    rootTable = new Info_Hash();

    Added = -1;
    Deleted = -1;
}
//function to create Chilren --- only used when we have to create a Children Matrix
void Trie_Node::createChildren(Trie_Node created)
{
    this->children = (Trie_Node *)malloc(sizeof(Trie_Node) * CHILDREN_MATRIX);
    for (int i = 0; i < CHILDREN_MATRIX; i++)
    {
        new (&(this->children[i])) Trie_Node();
    }
    this->children[0] = created;
    this->numberOfChildren++;
}

void Trie_Node::operator=(const Trie_Node &newObject)
{
    if (this != &newObject)
    {
        if (newObject.state == 1)
        {
            for (int i = 0; i < SMALL_WORD_SIZE; i++)
            {
                word[i] = newObject.word[i];
            }
        }
        else if (newObject.state == 2)
        {
            bigWord = (char *)malloc(strlen(newObject.bigWord) + 1);
            strncpy(bigWord, newObject.bigWord, strlen(newObject.bigWord) + 1);
            bigWord[strlen(newObject.bigWord)] = '\0';
        }
        children = newObject.children;
        state = newObject.state;
        timesDuplicated = newObject.timesDuplicated;
        isFinal = newObject.isFinal;
        numberOfChildren = newObject.numberOfChildren;

        toBeCompressed = newObject.toBeCompressed;

        Added = newObject.Added;
        Deleted = newObject.Deleted;
    }
}

void Trie_Node::Add(int countWords, char **words, int choice, int query_counter)
{
    int i = 0;
    Trie_Node *current = this;

    int check = 0;
    int exists = 0;

    if (choice == 1)
    {
        i = 1;
    }
    for (; i < countWords; i++)
    {
        char *word = words[i];

        //check = current->numberOfChildren;
        exists = 0;
        if ((choice == 1 && i > 1) || (choice == 0 && i > 0))
        {
            check = current->numberOfChildren;
        }
        else
        {
            check = 1;
        }
        if (check == 0)
        {
            //we havent allocated a matrix for the children of the node yet
            current->createChildren(Trie_Node(word));
            current->children[0].setAdded(query_counter);
            // current->children[0] = Trie_Node(word);
            current = &(current->children[0]);
        }
        else
        {
            int mid = 0;
            //binary search changing above linear search

            Trie_Node *temp = NULL;

            if (current->rootTable == NULL)
            {
                temp = binarySearch_for_Add(word, current->numberOfChildren, current->children, &mid);
            }
            else
            {
                temp = current->rootTable->searchNode(word);
            }

            //found the item with binary search
            if (temp != NULL)
            {
                current = temp;
                exists = 1;
            }

            //----------------------------------------------------

            if (exists == 0)
            {
                if (current->rootTable == NULL)
                {
                    //if we have not found our node in the children matrix
                    if (current->numberOfChildren == CHILDREN_MATRIX * (current->timesDuplicated + 1))
                    {
                        //we have to double our matrix
                        //we come in here if we dont have enough
                        //space for the children matrix
                        (current->timesDuplicated)++;
                        // cout << "realloc not in first layer " << endl;
                        // cout  << " times duplicated is : " << current->timesDuplicated << endl;
                        current->children = (Trie_Node *)realloc(current->children, (current->numberOfChildren) * sizeof(Trie_Node) * 2);
                        for (int w = current->numberOfChildren; w < (current->numberOfChildren) * 2; w++)
                        {
                            new (&(current->children[w])) Trie_Node();
                        }
                    }
                    //we start with the shifting of the Nodes
                    current->numberOfChildren++;

                    //try to shift with one memmmove
                    memmove(&(current->children[mid + 1]), &(current->children[mid]), ((current->numberOfChildren - mid) - 1) * sizeof(Trie_Node));

                    current->children[mid] = Trie_Node(word);
                    current->children[mid].setAdded(query_counter);

                    //------------------------------------------------

                    current = &(current->children[mid]);
                }
                else
                {
                    current = current->rootTable->addNode(word, query_counter);
                    // current->Print();
                }
            }
        }
        if (i == countWords - 1)
        {
            // if(strcmp(current->getWord(),"january") == 0)
            // cout<<"To fagame "<<query_counter<<endl;
            current->isFinal = true;

            if (exists == 1)
            {
                current->setAdded(query_counter);
            }
        }
    }
}

Trie_Node *Trie_Node::getChildren()
{
    return (this->children);
}

char *Trie_Node::getWord()
{
    return (this->word);
}

int Trie_Node::getNumberOfChildren()
{
    return (this->numberOfChildren);
}

int Trie_Node::getTimesDuplicated()
{
    return this->timesDuplicated;
}

unsigned short Trie_Node::Get_State()
{
    return (this->state);
}

void Trie_Node::Set_Big_Word(char *val)
{
    this->bigWord = val;
}

char *Trie_Node::Get_Big_Word()
{
    return (this->bigWord);
}

bool Trie_Node::Get_If_Final()
{
    return this->isFinal;
}

bool Trie_Node::Get_If_Printed()
{
    return this->isPrinted;
}

short *Trie_Node::Get_each_Word_Size_Array()
{

    return this->each_Word_Size_Array;
}

int Trie_Node::Get_Number_of_Words_in_SuperNode()
{

    return this->numbeOfWordsInSuperNode;
}

void Trie_Node::Set_Printed(bool val)
{
    this->isPrinted = val;
}

void Trie_Node::Set_Final(bool val)
{
    this->isFinal = val;
}

void Trie_Node::Set_State(unsigned short val)
{
    this->state = val;
}

//HelperFunction that Prints the Trie (SECOND EDITION)
void Trie_Node::Print()
{
    Trie_Node *temp = this;
    if (temp->children == NULL)
    {
        return;
    }
    cout << "---Node printing---" << endl;
    for (int i = 0; i < temp->numberOfChildren; i++)
    {
        if (temp->Get_State() == 1)
            cout << temp->children[i].word << endl;
        if (temp->Get_State() == 2)
            cout << temp->children[i].Get_Big_Word() << endl;

        cout << temp->numberOfChildren << endl;
        temp->children[i].Print();
        cout << endl;
    }
    cout << "---Node printing ending---" << endl;
}

int Trie_Node::RemoveFinal()
{
    if (this->isFinal == false)
    {
        return -1; //wasn't final to begin with
    }
    this->isFinal = false;
    return 0;
}

void Trie_Node::clearSelf()
{
    this->numberOfChildren = 0;
    this->isFinal = false;
    this->timesDuplicated = 0;
    if (this->state == 2)
    {
        free(this->bigWord);
        this->bigWord = NULL;
    }
    this->state = 0;
    this->isPrinted = false;
    this->children = NULL;
    for (int i = 0; i < SMALL_WORD_SIZE; i++)
        word[i] = '\0';
}

//Main Remove Function
int Trie_Node::Remove(char **words, int countWords, int qc)
{
    // char **words = NULL;
    // int countWords = 0;
    int countMatchedWords = 0;
    // GetLineInfo(line, countWords, words);

    int i = 0;
    Trie_Node *current = this;
    //Trie_Node *next;
    Trie_Node *parent;
    Trie_Node *traversed[countWords];
    int traversedIterator = 0;

    // int check = 0;
    int found = 0;
    int foundFinal = 0;
    int isRemoved = 0;
    int isFirst = 0;

    // check = current->numberOfChildren;
    // if (check == 0)
    // {
    //     return -1; // Tree is empty
    // }
    for (i = 1; i < countWords; i++)
    {
        char *word = words[i];
        if (current->rootTable == NULL)
        {
            int mid = 0;
            Trie_Node *tempCurr = binarySearch_for_Add(word, current->getNumberOfChildren(), current->children, &mid);
            if (tempCurr != NULL)
            {
                current = tempCurr;
                traversed[traversedIterator] = current;
                traversedIterator++;
                countMatchedWords++;
                found = 1;
                if (countMatchedWords == countWords - 1)
                {
                    foundFinal = 1;
                }
            }
        }
        else
        {
            current = rootTable->searchNode(word);

            if (current == NULL)
            {
                found = 0;
            }
            else
            {
                traversed[traversedIterator] = current;
                traversedIterator++;
                countMatchedWords++;
                if (countMatchedWords == countWords - 1)
                {
                    foundFinal = 1;
                }
                found = 1;
            }
        }

        //no corresponding word was found so ngram cant exist
        if (found == 0)
        {
            // FreeInfo(words, countWords);
            return -1;
        }

        //matching final word of ngram was found
        if (foundFinal == 1)
        {
            break;
        }
        found = 0;

        //If next node doesnt have children then n-gram mustn't exist even though we found a mathcing one at the last pass
        if (current->numberOfChildren == 0)
        {
            // FreeInfo(words, countWords);
            return -1;
        }
    }
    //If we get here we have found the ngram

    //Checking if it is part of bigger ngram
    // if (current->numberOfChildren != 0)
    // {
    //current->isFinal = false; //have an actual function for that
    current->Deleted = qc;
    // FreeInfo(words, countWords);
    return 0;
    // }

    //If we get here node is final and needs removing, currentt will first be removed in th while
    //and then we check if next need removing
    traversedIterator--; //make iterator look at the last node which is the final at the start
    while (1)
    {
        traversedIterator--; //point to parent of currentode
        parent = traversed[traversedIterator];
        if (traversedIterator < 0)
        {
            parent = this;
            isFirst = 1;
        }
        else
        {
            parent = traversed[traversedIterator];
        }

        if (isFirst == 1)
        {
            parent->rootTable->deleteNode(current->word);

            // FreeInfo(words, countWords);
            return 0;
        }
        //If parent has no other children
        if (parent->numberOfChildren == 1)
        {
            if (current->children != NULL)
                free(current->children);
            parent->children[0].clearSelf();
            //Even if it hasn't got children left it is final sos we leave it be
            if (isFirst == 1)
            {
                parent->numberOfChildren--;
                return 0;
            }
            if (parent->isFinal == true)
            {
                free(parent->children);
                parent->children = NULL;
                parent->numberOfChildren--;
                // FreeInfo(words, countWords);
                return 0;
            }
            //If we reach here parent node isn't final more has
            //more children so it must be removed we redo the while then
            current = parent;
            continue;
            //if parent has more children we leave him be regardless
            //and put other children in the right place
        }
        else if (parent->numberOfChildren > 1)
        {
            if (current->children != NULL)
                free(current->children);

            for (i = 0; i < parent->numberOfChildren; i++)
            {

                //If we removed we need to move the ones on the right one seat on the left
                if (isRemoved == 1)
                {
                    parent->children[i - 1] = parent->children[i];
                    parent->children[i].clearSelf();
                    continue;
                }
                //If will certainly find a match we know next is kid of parent
                if (parent->children[i].state == 1)
                {
                    if (strcmp(parent->children[i].word, current->word) == 0)
                    {
                        parent->children[i].clearSelf();
                        isRemoved = 1;
                    }
                }
                else if (parent->children[i].state == 2)
                {
                    if (parent->children[i].bigWord != NULL && current->bigWord != NULL)
                        if (strcmp(parent->children[i].bigWord, current->bigWord) == 0)
                        {
                            parent->children[i].clearSelf();
                            isRemoved = 1;
                        }
                }
            }
            parent->numberOfChildren--;
            // FreeInfo(words, countWords);
            return 1;
        }
        else
        {
            cout << "Parent doesnt have children impossiible error " << endl;
            // FreeInfo(words, countWords);
            return -1; //error not possible
        }
    }
}

Trie_Node::~Trie_Node()
{

    if (this->state == 2)
    {
        if (bigWord != NULL)
        {
            free(bigWord);
            bigWord = NULL;
        }
    }

    if (each_Word_Size_Array != NULL)
    {
        free(each_Word_Size_Array);
        each_Word_Size_Array = NULL;
    }

    if (rootTable != NULL)
        delete rootTable;
}

//-------------------------------FOR COMPRESSED TRIE------------------------------------------

//Holds a path of Trie nodes to be compressed
void Trie_Node::Add_To_Compression_Path(Trie_Node **&Matrix, int SZE, Trie_Node *&pToTrieNode)
{
    int i;
    for (i = 0; i < SZE; i++)
    {
        if (Matrix[i] == NULL)
        {
            Matrix[i] = pToTrieNode;
            break;
        }
    }

    if (i == SZE)
    {
        //if we have need more space
        //we duplicate our matrix
        Path_To_CompressTimesDuplicated++;
        Matrix = (Trie_Node **)realloc(Matrix, 2 * sizeof(Trie_Node *) * SZE);
        for (int j = SZE; j < SZE * 2; j++)
        {
            Matrix[j] = NULL;
        }

        Matrix[i] = pToTrieNode;
        ///////////
    }
}

//Reset the matrix the path of nodes to be compressed
void Trie_Node::Clean_Compression_Path(Trie_Node **&Matrix, int SZE)
{

    for (int i = 0; i < SZE; i++)
    {
        if (i == SZE - 1)
        {
            Matrix[i] = NULL;
        }
        else
        {
            if (Matrix[i] != NULL)
            {
                free(Matrix[i]);
                Matrix[i] = NULL;
            }
            else
                break;
        }
    }
}

void Trie_Node::Compress_Path_and_Insert_To_SuperNode(Trie_Node **&Matrix, int pathLen, int concatenatedSentenceLen, Trie_Node *&superNode)
{

    //creating the short array that holds the lengths of each word
    //pathLen + 1 cause it will hold the len of the word that superNode originally had.
    if (superNode->each_Word_Size_Array == NULL)
    {
        superNode->each_Word_Size_Array = (short *)malloc((pathLen + 1) * sizeof(short));
        superNode->numbeOfWordsInSuperNode = pathLen + 1;
    }

    //checking the state of superNode
    //and initilizing length and word arrays

    if (superNode->state == 1)
    { //has small word

        superNode->bigWord = (char *)malloc((concatenatedSentenceLen + strlen(superNode->word)) * sizeof(char) + 1); //7
        memset(superNode->bigWord, '\0', (concatenatedSentenceLen + strlen(superNode->word)) * sizeof(char) + 1);    //7

        strcpy(superNode->bigWord, superNode->word);

        // memset(superNode->word,'\0',strlen(superNode->word));

        //the len of the supernode word before the compression
        //negative value cause a super node can never be final node.

        if (superNode->isFinal == true)
            superNode->each_Word_Size_Array[0] = strlen(superNode->word);
        else
            superNode->each_Word_Size_Array[0] = -strlen(superNode->word);

        //cause it holds a big word now
        superNode->state = 2;
    }
    else if (superNode->state == 2)
    {

        char *temp = (char *)malloc((concatenatedSentenceLen + strlen(superNode->bigWord)) * sizeof(char) + 1); //7
        memset(temp, '\0', concatenatedSentenceLen + strlen(superNode->bigWord) + 1);                           //7

        strcpy(temp, superNode->bigWord);
        free(superNode->bigWord);
        superNode->bigWord = temp;

        if (superNode->isFinal == true)
            superNode->each_Word_Size_Array[0] = strlen(superNode->bigWord);
        else
            superNode->each_Word_Size_Array[0] = -strlen(superNode->bigWord);
    }

    //put the words from path to be compress into a concatenated sentence in supernode
    //and also update the array of shorts
    for (int i = pathLen - 1, array_pos = 1; i >= 0; i--, array_pos++)
    {

        //if the word to be concatenated is final
        if (Matrix[i]->isFinal == true)
        {

            if (Matrix[i]->state == 1)
            {

                superNode->each_Word_Size_Array[array_pos] = strlen(Matrix[i]->word);

                //only using bigWord now
                //don't have to ceck state
                strcat(superNode->bigWord, Matrix[i]->word);
            }
            else if (Matrix[i]->state == 2)
            {

                superNode->each_Word_Size_Array[array_pos] = strlen(Matrix[i]->bigWord);

                //only using bigWord now
                //don't have to ceck state
                strcat(superNode->bigWord, Matrix[i]->bigWord);
            }
        }
        //not final -> negative value for length
        else
        {

            if (Matrix[i]->state == 1)
            {

                superNode->each_Word_Size_Array[array_pos] = -strlen(Matrix[i]->word);

                //only using bigWord now
                //don't have to ceck state
                strcat(superNode->bigWord, Matrix[i]->word);
            }
            else if (Matrix[i]->state == 2)
            {

                superNode->each_Word_Size_Array[array_pos] = -strlen(Matrix[i]->bigWord);

                //only using bigWord now
                //don't have to ceck state
                strcat(superNode->bigWord, Matrix[i]->bigWord);
            }
        }
        if (i == 0)
        {
            if ((Matrix[0]->numberOfChildren) > 0)
            {
                // memcpy(superNode->children, Matrix[0]->children, (Matrix[0]->numberOfChildren) * sizeof(Trie_Node));
                superNode->numberOfChildren = Matrix[0]->numberOfChildren;
                superNode->children = Matrix[0]->children;
                superNode->timesDuplicated = Matrix[0]->timesDuplicated;
            }
            else
            {
                superNode->children = NULL;
                superNode->numberOfChildren = 0;
                superNode->isFinal = true;
            }
        }
    }
}

void Trie_Node::Compress_Trie(Trie_Node **&Matrix, int SZE)
{

    Trie_Node *root = this;

    static Trie_Node *Root = this;

    //the node that the other nodes will be compressed into
    Trie_Node *superNode = NULL;

    //counts the length of the path to be compressed
    static int pathLen = 0;
    //hold the len of each word concatenated sentence
    static int concatenatedSentenceLen = 0;

    if (root != NULL)
    {
        if (root->rootTable == NULL)
        {
            Trie_Node *current = root->children;

            for (int i = 0; i < (root->numberOfChildren); i++)
            {

                if (((current->state) != 0))
                {
                    if (root == Root)
                    {

                        if ((current != NULL) && (current->numberOfChildren) == 1 && (current->state > 0))
                        {
                            superNode = current;
                        }
                    }

                    else
                    {

                        if ((current != NULL) && (current->numberOfChildren) == 1 && (root->numberOfChildren > 1) && (current->state > 0))
                        {
                            superNode = current;
                            // cout <<"Super Node : "<< superNode->word <<endl;
                        }
                    }

                    if ((superNode != current) && (root->numberOfChildren == 1))
                    {
                        current->toBeCompressed = true;
                    }

                    current->Compress_Trie(Matrix, SZE);

                    if (current->toBeCompressed == true)
                    {
                        Add_To_Compression_Path(Matrix, SZE, current);

                        pathLen++;

                        if (current->state == 1)
                            concatenatedSentenceLen = concatenatedSentenceLen + strlen(current->word);
                        else if (current->state == 2)
                            concatenatedSentenceLen = concatenatedSentenceLen + strlen(current->bigWord);
                    }

                    if ((superNode != NULL) && (superNode == current))
                    {

                        Compress_Path_and_Insert_To_SuperNode(Matrix, pathLen, concatenatedSentenceLen, superNode);

                        Clean_Compression_Path(Matrix, SZE);
                        pathLen = 0;
                        concatenatedSentenceLen = 0;
                        superNode = NULL;
                    }

                    current++;
                }
            }
        }
        else
        {
            int h = pow(2, root->rootTable->round);
            for (int i = 0; i < HASH_SIZE * h + root->rootTable->tosplit; i++)
            {
                Trie_Node *temp = root->rootTable->buckets[i].table;
                int h1 = pow(2, root->rootTable->buckets[i].timesSplit);
                for (int j = 0; j < h1 * HASH_BUCKET_SIZE; j++)
                {
                    if (temp[j].state == 0)
                    {
                        break;
                    }
                    else
                    {
                        if (root == Root)
                        {

                            if ((temp[j].numberOfChildren) == 1)
                            {
                                superNode = &temp[j];
                            }
                        }

                        temp[j].Compress_Trie(Matrix, SZE);
                        if ((superNode != NULL) && (superNode == &temp[j]))
                        {

                            Compress_Path_and_Insert_To_SuperNode(Matrix, pathLen, concatenatedSentenceLen, superNode);

                            Clean_Compression_Path(Matrix, SZE);
                            pathLen = 0;
                            concatenatedSentenceLen = 0;
                            superNode = NULL;
                        }
                    }
                }
            }
        }
    }
}

//HelperFunction that prints with the DFS method
void Trie_Node::print_Compressed_Trie()
{

    Trie_Node *root = this;

    if (root != NULL)
    {
        if (root->rootTable == NULL)
        {
            Trie_Node *current = root->children;

            for (int i = 0; i < (root->numberOfChildren); i++)
            {

                if (current->state != 0)
                {

                    if (current->state == 1)
                        cout << current->word << endl;
                    else if (current->state == 2)
                    {
                        cout << current->bigWord << endl;

                        if (current->each_Word_Size_Array != NULL)
                        {
                            cout << "number_of_words_is " << current->numbeOfWordsInSuperNode << " " << current->each_Word_Size_Array[0] << endl;
                        }
                    }

                    if ((current->isFinal) == true)
                        cout << "Final" << endl;

                    current->print_Compressed_Trie();
                    current++;
                }
            }
        }
        else
        {
            int h = pow(2, root->rootTable->round);
            for (int i = 0; i < HASH_SIZE * h + root->rootTable->tosplit; i++)
            {
                Trie_Node *temp = root->rootTable->buckets[i].table;
                int h1 = pow(2, root->rootTable->buckets[i].timesSplit);
                for (int j = 0; j < h1 * HASH_BUCKET_SIZE; j++)
                {
                    if (temp[j].state == 0)
                        break;

                    if (temp[j].Get_State() == 1)
                        cout << temp[j].getWord() << endl;
                    if (temp[j].state == 2)
                        cout << temp[j].bigWord << endl;

                    temp[j].print_Compressed_Trie();
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------