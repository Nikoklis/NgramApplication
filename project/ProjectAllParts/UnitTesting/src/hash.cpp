#include "trie.h"
#include "HelperFunctions.h"
#include "functions.h"

Info_Hash::Info_Hash()
{
    round = 0;
    tosplit = 0;
    createBucket(true);
}

Info_Hash::~Info_Hash()
{
}

Hash_Bucket::~Hash_Bucket()
{
}
int Info_Hash::createBucket(bool isStart)
{
    ///////At the start of the table when we initialize it
    if (isStart == true)
    {
        this->buckets = (Hash_Bucket *)malloc(sizeof(Hash_Bucket) * HASH_SIZE);
        for (int i = 0; i < HASH_SIZE; i++)
        {
            new (&(this->buckets[i])) Hash_Bucket();
        }
        return 0;
    }
    ///////EveryOtherTIme
    int sup = pow(2, round);
    this->buckets = (Hash_Bucket *)realloc(this->buckets, (sizeof(Hash_Bucket) * ((HASH_SIZE * sup) + tosplit + 1)));
    new (&(this->buckets[(HASH_SIZE * sup) + tosplit])) Hash_Bucket();
    //tosplit++;          //We gonna split the next
    return 0;
}

Trie_Node *Info_Hash::searchNode(char *word)
{

    uint64_t h = hashFunc(word);
    //int flag=0;
    int h1 = pow(2, round) * HASH_SIZE;
    int h2 = pow(2, round + 1) * HASH_SIZE;
    int hashValue1 = h % h1;
    int hashValue2 = h % h2;
    //We get the bucket pointed by first hash
    Hash_Bucket *temp = (&buckets[hashValue1]);
    //we iterate through first bucket to find the match
    int mid = 0;

    Trie_Node *tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);

    if (tempCurr != NULL)
    {
        return tempCurr;
    }

    //IF THIS IS TRUE we should have already found it since
    //even if  use the second hash it points to shame bucket
    //So it doesnt exist
    if (hashValue2 == hashValue1)
    {
        return NULL;
    }
    //If this is true the second block doesnt exist
    // so we should have found it since the second respective bucket doesnt exist
    if (hashValue2 >= h1 + tosplit)
    {
        return NULL;
    }
    //There is a bucket to check but we still dont know if it exists
    //SO we repeat the steps from before
    temp = (&buckets[hashValue2]);

    //we iterate through first bucket to find the match
    tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);
    if (tempCurr != NULL)
    {
        return tempCurr;
    }

    

    //We didn't find t so it doesn't exist
    return NULL;
}

uint64_t Info_Hash::hashFunc(char *word)
{
    uint64_t murmur_Hash[2];
    uint64_t h1;

    MurmurHash3_x64_128(word, strlen(word), 0, murmur_Hash);

    h1 = murmur_Hash[1];
    return h1;
}

Trie_Node *Info_Hash::addNode(char *word,int query_counter)
{
    uint64_t h = hashFunc(word);

    int mysup = pow(2, round);
    int mysup2 = pow(2, round + 1);

    int h1 = mysup * HASH_SIZE;
    int h2 = mysup2 * HASH_SIZE;

    int hashValue1 = h % h1;
    int hashValue2 = h % h2;
    Hash_Bucket *temp = NULL;

    if (hashValue2 > (h1 + tosplit - 1))
    {
        temp = (&buckets[hashValue1]);
    }
    else
    {
        temp = (&buckets[hashValue2]);
    }
    //checking the bucket and it overflows

    //If true we have space so we add it
    //We change  here for ensuring alphbetical order
    int mid = 0;
    int hAdd = pow(2, temp->timesSplit);
    Trie_Node *tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);

    if (tempCurr != NULL)
    {
        return (&(tempCurr[mid]));
    }

    if (temp->numberOfNodes < HASH_BUCKET_SIZE * hAdd)
    {
        //memmove(&(current->children[mid + 1]), &(current->children[mid]),((current->numberOfChildren - mid) - 1) * sizeof(Trie_Node));

        tempCurr = temp->table;
        if (mid != temp->numberOfNodes)
        {
            memmove(&(tempCurr[mid + 1]), &(tempCurr[mid]),
                    ((temp->numberOfNodes - mid)) * sizeof(Trie_Node));
        }
        temp->table[mid] = Trie_Node(word);
        temp->table[mid].setAdded(query_counter);
        temp->numberOfNodes++;
        // if (temp->numberOfNodes >= )
        // cout << " Bucket has : " << temp->numberOfNodes << endl;
        return (&(temp->table[mid]));
    }
    //Bucket is full so we split
    //If we reach here it means we need overflow so we need to "add bucket"/realloc and/or change round
    this->createBucket(false);
    Hash_Bucket *newBucket = &(buckets[h1 + tosplit]);
    Hash_Bucket *oldBucket = &(buckets[tosplit]);


    //We traverse the old bucket and move everything back so  both buckets are ready
    int j = 0;
    int flag1 = 0;
    unsigned int h3 = pow(2, round + 1) * HASH_SIZE;
    uint64_t h5 = 0;

    int supSplitOld = pow(2, oldBucket->timesSplit);
    int supSplitNew = pow(2, newBucket->timesSplit);
    int h4 = 0;
    int checkerOld = HASH_BUCKET_SIZE * supSplitOld;
    int checkerNew = HASH_BUCKET_SIZE * supSplitNew;
    for (int i = 0; i < checkerOld; i++)
    {
        switch (oldBucket->table[i].Get_State())
        {
        case 0:
            //Means we are done since there are no more entries
            flag1 = 1;
            break;
        case 1:
            h5 = hashFunc(oldBucket->table[i].getWord());

            h4 = h5 % h3;
            if (h4 == tosplit)
            {
                break;
            }
            else
            {
                if (j == checkerNew)
                {
                    //we add the times split to indicate we must double the table
                    newBucket->timesSplit++;
                    supSplitNew = pow(2, newBucket->timesSplit);
                    checkerNew = HASH_BUCKET_SIZE * supSplitNew;
                    // cout << "bucket size " <<  << " lol bucket "<<h1+tosplit << endl;
                    newBucket->table = (Trie_Node *)realloc(newBucket->table, sizeof(Trie_Node) * checkerNew);
                    for (int i = (checkerNew) / 2; i < (checkerNew); i++)
                    {
                        new (&newBucket->table[i]) Trie_Node();
                    }
                }
                memcpy(&(newBucket->table[j]), &(oldBucket->table[i]), sizeof(Trie_Node));
                newBucket->numberOfNodes++;
                j++;
                
                if (checkerOld - (i + 1) != 0)
                {
                    memmove(&(oldBucket->table[i]), &(oldBucket->table[i + 1]), (checkerOld - (i + 1)) * sizeof(Trie_Node));
                    i--;
                }
                if (oldBucket->table[oldBucket->numberOfNodes - 1].Get_State() == 2)
                {
                    free(oldBucket->table[oldBucket->numberOfNodes - 1].Get_Big_Word());
                }
                oldBucket->table[oldBucket->numberOfNodes - 1].Set_State(0);

                oldBucket->numberOfNodes--;

            }
            break;
        case 2:
            h5 = hashFunc(oldBucket->table[i].Get_Big_Word());
            h4 = h5 % h3;
            if (h4 == tosplit)
            {
                break;
            }
            else
            {
                if (j == checkerNew)
                {
                    //we add the times split to indicate we must double the table
                    newBucket->timesSplit++;
                    supSplitNew = pow(2, newBucket->timesSplit);
                    checkerNew = HASH_BUCKET_SIZE * supSplitNew;
                    newBucket->table = (Trie_Node *)realloc(newBucket->table, sizeof(Trie_Node) * checkerNew);
                    for (int i = (checkerNew) / 2; i < (checkerNew); i++)
                    {
                        new (&newBucket->table[i]) Trie_Node();
                    }
                }
                memcpy(&(newBucket->table[j]), &(oldBucket->table[i]), sizeof(Trie_Node));
                newBucket->numberOfNodes++;
                j++;
                if (checkerOld - (i + 1) != 0)
                {
                    memmove(&(oldBucket->table[i]), &(oldBucket->table[i + 1]), (checkerOld - (i + 1)) * sizeof(Trie_Node));
                    i--;
                }
                //we remove the last one still left over from the move
                //if it doesnt have big word we just change the state
                //if it has we must free it
                if (oldBucket->table[oldBucket->numberOfNodes - 1].Get_State() == 2)
                {
                    free(oldBucket->table[oldBucket->numberOfNodes - 1].Get_Big_Word());
                }
                oldBucket->table[oldBucket->numberOfNodes - 1].Set_State(0);
                oldBucket->numberOfNodes--;
            }
            break;
        default:
            flag1 = 1;
            break;
        }
        if (flag1 == 1)
            break;
    }
    //We now add the new node
    //Will be true if this was the bucket split
    if (hashValue2 > (h1 + tosplit))
    {
        temp = (&buckets[hashValue1]);
    }
    else
    {
        temp = (&buckets[hashValue2]);
    }
    hAdd = pow(2, temp->timesSplit);
    mid = 0;
    tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);

    if (temp->numberOfNodes >= HASH_BUCKET_SIZE * hAdd)
    {
        temp->timesSplit++;
        hAdd = pow(2, temp->timesSplit);
        temp->table = (Trie_Node *)realloc(temp->table, sizeof(Trie_Node) * HASH_BUCKET_SIZE * hAdd);
        for (int i = (HASH_BUCKET_SIZE * hAdd) / 2; i < (HASH_BUCKET_SIZE * hAdd); i++)
        {
            new (&temp->table[i]) Trie_Node();
        }
        // temp->table[temp->numberOfNodes] = Trie_Node(word);
        // temp->numberOfNodes++;
    }
    tempCurr = temp->table;
    if (mid != temp->numberOfNodes)
    {
        memmove(&(tempCurr[mid + 1]), &(tempCurr[mid]),
                ((temp->numberOfNodes - mid)) * sizeof(Trie_Node));
    }
    temp->table[mid] = Trie_Node(word);
    temp->table[mid].setAdded(query_counter); 

    temp->numberOfNodes++;

    this->tosplit++;
    int lastSup = pow(2, round);
    if (this->tosplit == HASH_SIZE * lastSup)
    {
        this->round++;
        // cout <<"new round is :"<< this->round << endl;
        this->tosplit = 0;
    }
    return (&(temp->table[mid]));
}

int Info_Hash::deleteNode(char *word)
{
    uint64_t h = hashFunc(word);
    //    int flag=0;
    int h1 = pow(2, round) * HASH_SIZE;
    int h2 = pow(2, round + 1) * HASH_SIZE;
    int hashValue1 = h % h1;
    int hashValue2 = h % h2;
    //We get the bucket pointed by first hash
    Hash_Bucket *temp = NULL;
    // int mid = 0;

    if (hashValue2 > (h1 + tosplit - 1))
    {
        temp = (&buckets[hashValue1]);
    }
    else
    {
        temp = (&buckets[hashValue2]);
    }
    //Trie_Node *table= (&temp->table[0]);

    int sup = pow(2, temp->timesSplit);
    // Trie_Node *tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);

    // if (tempCurr != NULL)
    // {
    //     if (HASH_BUCKET_SIZE * sup > mid + 1)
    //     {
    //         memmove(&(temp->table[mid]), &(temp->table[mid + 1]), (HASH_BUCKET_SIZE * sup - (mid + 1)) * sizeof(Trie_Node));
    //     }
    //     //we remove the last one still left over from the move
    //     //if it doesnt have big word we just change the state
    //     //if it has we must free it
    //     if (temp->table[temp->numberOfNodes - 1].Get_State() == 2)
    //     {
    //         free(temp->table[temp->numberOfNodes - 1].Get_Big_Word());
    //     }
    //     temp->numberOfNodes--;
    //     temp->table[temp->numberOfNodes].Set_State(0);

    //     return 0;
    // }
    int checkerLoop = HASH_BUCKET_SIZE * sup;
    for (int i = 0; i < checkerLoop; i++)
    {
        if (temp->table[i].Get_State() == 0)
        {
            break;
        }
        if (temp->table[i].Get_State() == 1)
        {
            if (strcmp(temp->table[i].getWord(), word) == 0)
            {
                if (checkerLoop > i + 1)
                {
                    memmove(&(temp->table[i]), &(temp->table[i + 1]), (checkerLoop - (i + 1)) * sizeof(Trie_Node));
                }
                //we remove the last one still left over from the move
                //if it doesnt have big word we just change the state
                //if it has we must free it
                if (temp->table[temp->numberOfNodes - 1].Get_State() == 2)
                {
                    free(temp->table[temp->numberOfNodes - 1].Get_Big_Word());
                }
                temp->numberOfNodes--;
                temp->table[temp->numberOfNodes].Set_State(0);

                return 0;
            }
        }
        if (temp->table[i].Get_State() == 2)
        {
            if (strcmp(temp->table[i].getWord(), word) == 0)
            {
                if (checkerLoop > i + 1)
                    memmove(&(temp->table[i]), &(temp->table[i + 1]), (checkerLoop - (i + 1)) * sizeof(Trie_Node));

                //we remove the last one still left over from the move
                //if it doesnt have big word we just change the state
                //if it has we must free it
                if (temp->table[temp->numberOfNodes - 1].Get_State() == 2)
                {
                    free(temp->table[temp->numberOfNodes - 1].Get_Big_Word());
                }
                temp->numberOfNodes--;
                temp->table[temp->numberOfNodes].Set_State(0);

                return 0;
            }
        }
    }

    //IF THIS IS TRUE we should have already found it since even if  use the second hash it points to shame bucket
    //So it doesnt exist
    if (hashValue2 < h1)
    {
        return -1;
    }
    //If this is true the second block doesnt exist so we should have found it since the second respective bucket doesnt exist
    if (hashValue2 >= h1 + tosplit)
    {
        return -1;
    }
    //There is a bucket to check but we still dont know if it exists
    //SO we repeat the steps from before
    if (hashValue2 > (h1 + tosplit))
    {
        temp = (&buckets[hashValue1]);
        // if (tosplit == hashValue1)
        // {
        //     tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);
        // }
    }
    else
    {
        temp = (&buckets[hashValue2]);
        // tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);
    }
    sup = pow(2, temp->timesSplit);
    // tempCurr = binarySearch_for_Add(word, temp->numberOfNodes, temp->table, &mid);
    // if (tempCurr != NULL)
    // {

    //     if (HASH_BUCKET_SIZE * sup > mid + 1)
    //         memmove(&(temp->table[mid]), &(temp->table[mid + 1]), (HASH_BUCKET_SIZE * sup - (mid + 1)) * sizeof(Trie_Node));

    //     //we remove the last one still left over from the move
    //     //if it doesnt have big word we just change the state
    //     //if it has we must free it
    //     if (temp->table[temp->numberOfNodes - 1].Get_State() == 2)
    //     {
    //         free(temp->table[temp->numberOfNodes - 1].Get_Big_Word());
    //     }
    //     temp->numberOfNodes--;
    //     temp->table[temp->numberOfNodes].Set_State(0);
    //     return 0;
    // }
    checkerLoop = HASH_BUCKET_SIZE *sup;
    //we iterate through first bucket to find the match
    for (int i = 0; i < checkerLoop; i++)
    {
        if (temp->table[i].Get_State() == 0)
        {
            break;
        }
        if (temp->table[i].Get_State() == 1)
        {
            if (strcmp(temp->table[i].getWord(), word) == 0)
            {
                if (strcmp(temp->table[i].getWord(), word) == 0)
                {
                    if (checkerLoop > i + 1)
                        memmove(&(temp->table[i]), &(temp->table[i + 1]), (checkerLoop - (i + 1)) * sizeof(Trie_Node));

                    //we remove the last one still left over from the move
                    //if it doesnt have big word we just change the state
                    //if it has we must free it
                    if (temp->table[temp->numberOfNodes - 1].Get_State() == 2)
                    {
                        free(temp->table[temp->numberOfNodes - 1].Get_Big_Word());
                    }
                    temp->numberOfNodes--;
                    temp->table[temp->numberOfNodes].Set_State(0);
                    return 0;
                }
            }
        }
        if (temp->table[i].Get_State() == 2)
        {
            if (strcmp(temp->table[i].Get_Big_Word(), word) == 0)
            {
                if (strcmp(temp->table[i].getWord(), word) == 0)
                {
                    if (checkerLoop > i + 1)
                        memmove(&(temp->table[i]), &(temp->table[i + 1]), (checkerLoop - (i + 1)) * sizeof(Trie_Node));

                    //we remove the last one still left over from the move
                    //if it doesnt have big word we just change the state
                    //if it has we must free it
                    if (temp->table[temp->numberOfNodes - 1].Get_State() == 2)
                    {
                        free(temp->table[temp->numberOfNodes - 1].Get_Big_Word());
                    }
                    temp->numberOfNodes--;
                    temp->table[temp->numberOfNodes].Set_State(0);
                    return 0;
                }
            }
        }
    }

    //We didn't find t so it doesn't exist
    return -1;
}

Hash_Bucket::Hash_Bucket()
{
    numberOfNodes = 0;
    isOverflow = false;
    timesSplit = 0;
    this->table = (Trie_Node *)malloc(sizeof(Trie_Node) * HASH_BUCKET_SIZE);
    for (int i = 0; i < HASH_BUCKET_SIZE; i++)
    {
        new (&(this->table[i])) Trie_Node();
    }
}
