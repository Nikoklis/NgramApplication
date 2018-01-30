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

void Trie_Node::Add(int countWords, char **words, int choice,int query_counter)
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
                    current = current->rootTable->addNode(word,query_counter);
                }
            }
        }
        if (i == countWords - 1)
        {
            current->isFinal = true;

            if(exists == 1){
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
int Trie_Node::Remove(char **words, int countWords,int qc)
{
    // char **words = NULL;
    // int countWords = 0;
    int countMatchedWords = 0;
    // GetLineInfo(line, countWords, words);

    int i = 0;
    Trie_Node *current = this;
    //Trie_Node *next;
    // Trie_Node *parent;
    Trie_Node *traversed[countWords];
    int traversedIterator = 0;

    // int check = 0;
    int found = 0;
    int foundFinal = 0;
    // int isRemoved = 0;
    // int isFirst = 0;

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
    current->Deleted=qc;
        // FreeInfo(words, countWords);
    return 0;
    // }

    //If we get here node is final and needs removing, currentt will first be removed in th while
    //and then we check if next need removing
    // traversedIterator--; //make iterator look at the last node which is the final at the start
    // while (1)
    // {
    //     traversedIterator--; //point to parent of currentode
    //     parent = traversed[traversedIterator];
    //     if (traversedIterator < 0)
    //     {
    //         parent = this;
    //         isFirst = 1;
    //     }
    //     else
    //     {
    //         parent = traversed[traversedIterator];
    //     }

    //     if (isFirst == 1)
    //     {
    //         parent->rootTable->deleteNode(current->word);

    //         // FreeInfo(words, countWords);
    //         return 0;
    //     }
    //     //If parent has no other children
    //     if (parent->numberOfChildren == 1)
    //     {
    //         if (current->children != NULL)
    //             free(current->children);
    //         parent->children[0].clearSelf();
    //         //Even if it hasn't got children left it is final sos we leave it be
    //         if (isFirst == 1)
    //         {
    //             parent->numberOfChildren--;
    //             return 0;
    //         }
    //         if (parent->isFinal == true)
    //         {
    //             free(parent->children);
    //             parent->children = NULL;
    //             parent->numberOfChildren--;
    //             // FreeInfo(words, countWords);
    //             return 0;
    //         }
    //         //If we reach here parent node isn't final more has
    //         //more children so it must be removed we redo the while then
    //         current = parent;
    //         continue;
    //         //if parent has more children we leave him be regardless
    //         //and put other children in the right place
    //     }
    //     else if (parent->numberOfChildren > 1)
    //     {
    //         if (current->children != NULL)
    //             free(current->children);

    //         for (i = 0; i < parent->numberOfChildren; i++)
    //         {

    //             //If we removed we need to move the ones on the right one seat on the left
    //             if (isRemoved == 1)
    //             {
    //                 parent->children[i - 1] = parent->children[i];
    //                 parent->children[i].clearSelf();
    //                 continue;
    //             }
    //             //If will certainly find a match we know next is kid of parent
    //             if (parent->children[i].state == 1)
    //             {
    //                 if (strcmp(parent->children[i].word, current->word) == 0)
    //                 {
    //                     parent->children[i].clearSelf();
    //                     isRemoved = 1;
    //                 }
    //             }
    //             else if (parent->children[i].state == 2)
    //             {
    //                 if (parent->children[i].bigWord != NULL && current->bigWord != NULL)
    //                     if (strcmp(parent->children[i].bigWord, current->bigWord) == 0)
    //                     {
    //                         parent->children[i].clearSelf();
    //                         isRemoved = 1;
    //                     }
    //             }
    //         }
    //         parent->numberOfChildren--;
    //         // FreeInfo(words, countWords);
    //         return 1;
    //     }
    //     else
    //     {
    //         cout << "Parent doesnt have children impossiible error " << endl;
    //         // FreeInfo(words, countWords);
    //         return -1; //error not possible
    //     }
    // }
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

        superNode->bigWord = (char *)malloc((concatenatedSentenceLen + strlen(superNode->word)) * sizeof(char) + 1);//7
        memset(superNode->bigWord, '\0', (concatenatedSentenceLen + strlen(superNode->word)) * sizeof(char) + 1);//7

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

        char *temp = (char *)malloc((concatenatedSentenceLen + strlen(superNode->bigWord)) * sizeof(char) + 1);//7
        memset(temp, '\0', concatenatedSentenceLen + strlen(superNode->bigWord) + 1);//7

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