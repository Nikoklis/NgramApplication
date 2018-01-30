#include "trie.h"
#include "functions.h"
#include "Jobs.h"
#include "HelperFunctions.h"

extern Parallel_Printing_Array *Th_Array;
extern int numberOfThreadsWaiting;

//returns first word of a string ....in heap
void takeWord(char **firstWord, char *str)
{
	int origLen, restLen, firstWordLen;
	char *space = NULL;
	//*firstWord = NULL;
	origLen = strlen(str);
	space = strchr(str, ' ');

	if (space != NULL)
	{ //if not last word

		restLen = strlen(space);
		firstWordLen = origLen - restLen;

		*firstWord = (char *)malloc(firstWordLen + 1);
		strncpy(*firstWord, str, firstWordLen);
		firstWord[0][firstWordLen] = '\0';
	}
}

//Binary search function in a Trie_Node matrix
Trie_Node *binarySearch_for_Add(char *w, int n, Trie_Node *x, int *pos)
{

	int cond, low, high, mid = 0;
	low = 0;		   /* Lower limit for searching */
	high = n - 1;	  /* Upper limit for searching */
	int sm_eq_big = 1; //0 if equal, 1 if smaller ,2 if bigger

	while (low <= high)
	{
		/* Do we have space for searching? */

		mid = (low + high) / 2; /* Medium element of search interval */
								/* to compare with word we are looking for */

		if (x[mid].Get_State() == 0)
		{
			//empty room to the right....continue to first half
			high = mid - 1;
			// sm_eq_big = 1;
		}
		else if (x[mid].Get_State() == 1)
		{
			//small word
			if ((cond = strcmp(w, x[mid].getWord())) < 0)
			{					/* Compare medium to word */
				high = mid - 1; /* Not found, word might be at first half */
				sm_eq_big = 1;
			}
			else if (cond > 0)
			{

				low = mid + 1; /* Not found, word might be at second half */
				sm_eq_big = 2;
			}
			else
			{
				return (&x[mid]); /* We found it! */
			}
		}
		else if (x[mid].Get_State() == 2)
		{
			//big word
			short *sizeArray;
			sizeArray = x[mid].Get_each_Word_Size_Array();

			//super node
			if (sizeArray != NULL)
			{

				//have to check is the word is bigger or smaller to the compared first word in the super node
				//and so i take the appropritae action
				//else i would have problems for example (red and reded)
				if (strlen(w) == (unsigned int)abs(sizeArray[0]))
				{

					cond = strncmp(w, x[mid].Get_Big_Word(), (unsigned int)abs(sizeArray[0]));

					if (cond < 0)
					{					/* Compare medium to word */
						high = mid - 1; /* Not found, word might be at first half */
					}
					else if (cond > 0)
					{

						low = mid + 1; /* Not found, word might be at second half */
					}
					else
					{
						return (&x[mid]); /* We found it! */
					}
				}
				else
				{

					char *temp = (char *)malloc(abs(sizeArray[0]) + 1);
					strncpy(temp, x[mid].Get_Big_Word(), (unsigned int)abs(sizeArray[0]));
					temp[(unsigned int)abs(sizeArray[0])] = '\0';

					cond = strcmp(w, temp);

					free(temp);

					if (cond < 0)
					{					/* Compare medium to word */
						high = mid - 1; /* Not found, word might be at first half */
					}
					else if (cond > 0)
					{

						low = mid + 1; /* Not found, word might be at second half */
					}
					// else
					// {
					// 	return (&x[mid]); /* We found it! */
					// }
				}
			}
			else
			{
				if ((cond = strcmp(w, x[mid].Get_Big_Word())) < 0)
				{					/* Compare medium to word */
					high = mid - 1; /* Not found, word might be at first half */
					sm_eq_big = 1;
				}
				else if (cond > 0)
				{

					low = mid + 1; /* Not found, word might be at second half */
					sm_eq_big = 2;
				}
				else
				{
					return (&x[mid]); /* We found it! */
				}
			}
		}
	}

	//the position of element to be inserted

	if (sm_eq_big == 1 || sm_eq_big == 0)
		*pos = mid;
	else if (sm_eq_big == 2)
		*pos = mid + 1;

	return NULL;
}

//Holds N-Grams to Be printed in a 2-dimensional Matrix
void insertToMatrix(Trie_Node **&Matrix, int SZE, Trie_Node *pToTrieNode, int *N_GramMatrixTimesDuplicated, int positionToInsert)
{
	if (Matrix[positionToInsert] == NULL)
		Matrix[positionToInsert] = pToTrieNode;
	// for (i = 0; i < SZE; i++)
	// {
	// 	if (Matrix[i] == NULL)
	// 	{

	// 		Matrix[i] = pToTrieNode;
	// 		break;
	// 	}
	// }

	if (positionToInsert == SZE)
	{
		//if we have need more space
		//we duplicate our matrix
		(*N_GramMatrixTimesDuplicated)++;

		Matrix = (Trie_Node **)realloc(Matrix, 2 * sizeof(Trie_Node *) * SZE);

		for (int j = SZE; j < SZE * 2; j++)
		{
			Matrix[j] = NULL;
		}

		Matrix[positionToInsert] = pToTrieNode;
		///////////
	}
}

//Print an N-Gram found in search with the correct format
void printN_GramMatrix(Trie_Node **&N_GramMatrix, int SZE, int numberOfFinalsToBePrinted)
{

	int i;

	for (i = 0; i < SZE; i++)
	{

		if (N_GramMatrix[i] != NULL)
		{
			if (N_GramMatrix[i]->Get_State() == 1)
			{
				cout << N_GramMatrix[i]->getWord();
			}
			else if (N_GramMatrix[i]->Get_State() == 2)
			{
				short *sizeArray = N_GramMatrix[i]->Get_each_Word_Size_Array();

				if (sizeArray != NULL && numberOfFinalsToBePrinted >= 0)
				{

					int num_of_words = N_GramMatrix[i]->Get_Number_of_Words_in_SuperNode();
					char *iterator = N_GramMatrix[i]->Get_Big_Word();

					for (int i = 0; i < num_of_words; i++)
					{

						if (i > 0)
							cout << " ";
						else
						{

							if (sizeArray[i] < 0 && numberOfFinalsToBePrinted > 0)
							{

								char *wtp = (char *)malloc((-sizeArray[i]) + 1);
								wtp[-sizeArray[i]] = '\0';

								strncpy(wtp, iterator, -sizeArray[i]);

								cout << wtp << endl;

								free(wtp);

								iterator = iterator + (-sizeArray[i]);

								numberOfFinalsToBePrinted--;
							}
							else
							{

								char *wtp = (char *)malloc(sizeArray[i] + 1);
								wtp[sizeArray[i]] = '\0';

								strncpy(wtp, iterator, sizeArray[i]);

								cout << wtp << endl;

								free(wtp);

								iterator = iterator + sizeArray[i];
							}
						}
					}
				}
				else
					cout << N_GramMatrix[i]->Get_Big_Word();
			}

			fflush(stdout);

			if (i < SZE - 1)
			{
				if (N_GramMatrix[i + 1] != NULL)
				{
					cout << " ";
					fflush(stdout);
				}
			}

			N_GramMatrix[i]->Set_Printed(true);
		}
	}
}

//Put the contents of the NGramMatrix in a string so it can be checked in the BloomFilter
void N_GramMatrix_to_String(Trie_Node **&N_GramMatrix, int SZE, char **ngram, bool includeLastSuperNode)
{
	int i;
	int last_SuperNode_Word_Size = 0;

	char *pts = strchr(*ngram, '\0');

	for (i = 0; i < SZE; i++)
	{

		if (N_GramMatrix[i] != NULL)
		{
			if (N_GramMatrix[i]->Get_State() == 1)
			{
				strcat(*ngram, N_GramMatrix[i]->getWord());
			}
			//big word and maybe superNode
			else if ((N_GramMatrix[i]->Get_State() == 2))
			{
				short *sizeArray = N_GramMatrix[i]->Get_each_Word_Size_Array();

				//means that i am in a superNode
				if (sizeArray != NULL)
				{

					int num_of_words = N_GramMatrix[i]->Get_Number_of_Words_in_SuperNode();
					char *iterator = N_GramMatrix[i]->Get_Big_Word();

					for (int i = 0; i < num_of_words; i++)
					{

						if (i > 0)
						{
							strcat(*ngram, " ");
							last_SuperNode_Word_Size++;
						}

						if (sizeArray[i] < 0)
						{

							strncat(*ngram, iterator, -sizeArray[i]);
							// cout<<"WWWWWW "<<*ngram<<endl;
							iterator = iterator + (-sizeArray[i]);
						}
						else
						{
							strncat(*ngram, iterator, sizeArray[i]);
							iterator = iterator + sizeArray[i];
						}

						last_SuperNode_Word_Size += (int)abs(sizeArray[i]);
					}
				}
				else
					strcat(*ngram, N_GramMatrix[i]->Get_Big_Word());
			}

			if (i < SZE - 1)
			{
				if (N_GramMatrix[i + 1] != NULL)
				{
					strcat(*ngram, " ");
					last_SuperNode_Word_Size++;
				}
			}
		}
		else
		{

			if (includeLastSuperNode == false)
			{

				memset(pts, '\0', last_SuperNode_Word_Size);
			}
		}
	}
}

//Reset the matrix that holds the N-Grams
void cleanN_GramMatrix(Trie_Node **&N_GramMatrix, int SZE)
{

	int i;

	for (i = 0; i < SZE; i++)
	{
		if (N_GramMatrix[i] != NULL)
		{
			// cout<<"CLEAN :"<<N_GramMatrix[i]->Get_Big_Word()<<endl;
			N_GramMatrix[i] = NULL;
		}
		else
			break;
	}
}

//Binary search function in a Trie_Node matrix
Trie_Node *binarySearch(char *w, int n, Trie_Node *x)
{

	int cond, low, high, mid = 0;
	low = 0;	  /* Lower limit for searching */
	high = n - 1; /* Upper limit for searching */

	while (low <= high)
	{
		/* Do we have space for searching? */

		mid = (low + high) / 2; /* Medium element of search interval */
								/* to compare with word we are looking for */

		if (x[mid].Get_State() == 0)
		{
			//empty room to the right....continue to first half
			high = mid - 1;
		}
		else if (x[mid].Get_State() == 1)
		{
			//small word
			if ((cond = strcmp(w, x[mid].getWord())) < 0)
			{					/* Compare medium to word */
				high = mid - 1; /* Not found, word might be at first half */
			}
			else if (cond > 0)
			{

				low = mid + 1; /* Not found, word might be at second half */
			}
			else
			{
				return (&x[mid]); /* We found it! */
			}
		}
		//also checking for super nodes
		else if (x[mid].Get_State() == 2)
		{
			short *sizeArray;
			sizeArray = x[mid].Get_each_Word_Size_Array();

			//super node
			if (sizeArray != NULL)
			{

				//have to check is the word is bigger or smaller to the compared first word in the super node
				//and so i take the appropritae action
				//else i would have problems for example (red and reded)
				if (strlen(w) == (unsigned int)abs(sizeArray[0]))
				{

					cond = strncmp(w, x[mid].Get_Big_Word(), (unsigned int)abs(sizeArray[0]));

					if (cond < 0)
					{					/* Compare medium to word */
						high = mid - 1; /* Not found, word might be at first half */
					}
					else if (cond > 0)
					{

						low = mid + 1; /* Not found, word might be at second half */
					}
					else
					{
						return (&x[mid]); /* We found it! */
					}
				}
				else
				{

					char *temp = (char *)malloc(abs(sizeArray[0]) + 1);
					strncpy(temp, x[mid].Get_Big_Word(), (unsigned int)abs(sizeArray[0]));
					temp[(unsigned int)abs(sizeArray[0])] = '\0';

					cond = strcmp(w, temp);

					free(temp);

					if (cond < 0)
					{					/* Compare medium to word */
						high = mid - 1; /* Not found, word might be at first half */
					}
					else if (cond > 0)
					{

						low = mid + 1; /* Not found, word might be at second half */
					}
					// else
					// {
					// 	return (&x[mid]); /* We found it! */
					// }
				}
			}
			//not a super node
			else
			{

				if ((cond = strcmp(w, x[mid].Get_Big_Word())) < 0)
				{					/* Compare medium to word */
					high = mid - 1; /* Not found, word might be at first half */
				}
				else if (cond > 0)
				{

					low = mid + 1; /* Not found, word might be at second half */
				}
				else
				{
					return (&x[mid]); /* We found it! */
				}
			}
		}
	}
	return NULL;
}


//HelperFunction that prints with the DFS method
void printTrie(Trie_Node *root)
{

	if (root != NULL)
	{

		Trie_Node *current = root->getChildren();
		for (int i = 0; i < (root->getNumberOfChildren()); i++)
		{

			if (((current->Get_State()) != 0))
			{
				if (current->Get_State() == 1)
				{
					cout << current->Get_State() << endl;
					cout << current->getWord() << endl;
				}

				if (current->Get_State() == 2)
				{
					cout << current->Get_State() << endl;
					cout << current->Get_Big_Word() << endl;
				}

				if ((current->Get_If_Final()) == true)
					cout << "final" << endl;
				printTrie(current);
				current++;
				cout << endl;
			}
		}
	}
}

//Frees the entire Struct of the Trie
void deleteTrie(Trie_Node *root)
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
					deleteTrie(current);
				current++;
			}

			if (root->getChildren() != NULL && numberOfChildren > 0)
			{
				Trie_Node *temp = root->getChildren();

				if (root->getNumberOfChildren() == 1)
				{
				}

				for (int i = 0; i < numberOfChildren; i++)
				{
					if (temp[i].Get_State() == 2)
					{
						if (temp[i].Get_Big_Word() != NULL)
						{
							free(temp[i].Get_Big_Word());
							temp[i].Set_Big_Word(NULL);
						}

						//only if it supernode
						if (temp[i].Get_each_Word_Size_Array() != NULL)
						{
							free(temp[i].Get_each_Word_Size_Array());
						}
					}
				}
				if (root != NULL && root->getChildren() != NULL)
					free(root->getChildren());
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
					deleteTrie((&current->table[i]));
			}

			for (int i = 0; i < (current->numberOfNodes); i++)
			{
				if (current->table[i].Get_State() == 2)
				{
					if (current->table[i].Get_Big_Word() != NULL)
					{
						free(current->table[i].Get_Big_Word());
						current->table[i].Set_Big_Word(NULL);
					}

					//only if it supernode
					if (current->table[i].Get_each_Word_Size_Array() != NULL)
					{
						free(current->table[i].Get_each_Word_Size_Array());
					}
				}
			}
			if (current != NULL)
			{
				if (current->table)
				{
					free(current->table);
				}
			}
		}
		free(root->rootTable->buckets);
	}
}

//--------------FOR COMPRESSED TRIE ONLY------------------------------------

void concatenate_N_Gram_Words(char *&concatenatedN_Gram, char *ngram, int num_of_words)
{

	char *space = NULL;
	char *multSpace = NULL;
	char *firstWord = NULL;
	bool flag = false;
	bool terminator = false;

	space = strchr(ngram, ' ');
	multSpace = space;

	for (int i = 1; i <= num_of_words; i++)
	{

		if (space != NULL) //not last word
		{

			do
			{
				multSpace++;
			} while (*multSpace == ' ');

			space = --multSpace;

			takeWord(&firstWord, ngram);
			flag = true;

			ngram = ++space;
		}
		else
		{
			firstWord = ngram;
			flag = false;
			terminator = true;
		}

		strcat(concatenatedN_Gram, firstWord);

		if (firstWord != NULL && flag == true)
			free(firstWord);

		space = strchr(ngram, ' ');
		multSpace = space;

		if (terminator == true)
			break;
	}
}

//putting the sentences of the superNode that finish with final at the end of the Ngram
//in order to be checked if they exist in the bloom filter
void final_In_SuperNode_to_String(char **str, char *superNode_words, short *&sizeArray, int number_of_words_in_superNode, int num_of_final)
{

	int counter = 0;
	char *pts = superNode_words;

	for (int i = 0; i < number_of_words_in_superNode; i++)
	{

		if (i > 0)
			strcat(*str, " ");

		strncat(*str, pts, (int)abs(sizeArray[i]));

		pts = pts + (int)abs(sizeArray[i]);

		//final
		if (sizeArray[i] > 0)
		{

			counter++;

			//found the final i want
			if (counter == num_of_final)
				break;
		}
	}
}

//Implenenatation of search that takes into account the super nodes.
void search_Compressed_Trie(Index *base, char *str, Heap &heap, int query_id, int query_MainCounter)
{
	char *firstWord = NULL;
	int positionToInsertInNGramMatrix = 0;
	bool freeFirstWord = false;
	char *next = str;	 //iterator based search on words
	char *iterator = str; //pointer to the Query string
	char *space = NULL;
	char *temp = NULL;
	char *multSpace = NULL; //to remove multiple spaces between words
							// int boolFlag = 0; //a flag to check how to free the firstWord space

	//matrix that holds N-Gram that is found
	int N_GramMatrixTimesDuplicated = 1;
	Trie_Node **N_GramMatrix;
	N_GramMatrix = (Trie_Node **)malloc(N_GRAM_MATRIX_SZE * sizeof(Trie_Node *));
	memset(N_GramMatrix, (int)NULL, N_GRAM_MATRIX_SZE * sizeof(Trie_Node *));

	BloomFilter *blF;
	int p = 0.001;
	int endianCheck = 1;
	// little endian if true
	if (*(char *)&endianCheck == 1)
		blF = new BloomFilter(false, p);
	else
		//big endian
		blF = new BloomFilter(true, p);

	Trie_Node *pToTrieNode = NULL;
	Trie_Node *current = base->getRoot();

	// bool firstLayer = true;
	bool found_N_Gram = false;
	// bool firstTime = false;
	int numberOfFinalsToBePrinted = 0;

	do
	{
		do
		{
			freeFirstWord = false;
			numberOfFinalsToBePrinted = 0;
			space = strchr(next, ' ');
			multSpace = space;
			//take first word of next
			if (space != NULL) //not last word
			{
				do
				{
					multSpace++;
				} while (*multSpace == ' ');

				space = --multSpace;

				freeFirstWord = true;
				takeWord(&firstWord, next);
			}
			else
			{
				firstWord = next;
			}

			//Search for word in first layer of the Trie
			if (current->rootTable == NULL)
			{
				if (current->getChildren() != NULL)
				{
					Trie_Node *p;
					p = current->getChildren();
					pToTrieNode = binarySearch(firstWord, (current->getTimesDuplicated() + 1) * CHILDREN_MATRIX, p);
				}
				else
				{
					pToTrieNode = NULL;
				}
			}
			else
			{
				pToTrieNode = current->rootTable->searchNode(firstWord);
			}

			//binary found something
			if (pToTrieNode != NULL)
			{

				short *sizeArray;

				sizeArray = pToTrieNode->Get_each_Word_Size_Array();

				//if found that a word of the NGram exists in a superNode....
				if (sizeArray != NULL)
				{

					//and so I have to  compare the rest of the words
					int num_of_words = pToTrieNode->Get_Number_of_Words_in_SuperNode();

					char *snw_it = pToTrieNode->Get_Big_Word();
					char *always_on_start = snw_it;
					char *temp_start = always_on_start;

					//moves on NGram
					char *ng_it = next;

					char *space = NULL;
					char *multSpace = NULL;
					bool boolFlag = false; //a flag to check how to move the "next" pointer
					// bool lastWordFlag = false;

					//not only the initial word of super node
					//then start to compare the rest of the words in super node with the NGram
					if (num_of_words > 1)
					{

						for (int w = 0; w < num_of_words; w++)
						{

							if (strlen(firstWord) == (unsigned)abs(sizeArray[w]))
							{

								if (strncmp(firstWord, snw_it, (unsigned)abs(sizeArray[w])) == 0)
								{

									//final in superNode
									if (sizeArray[w] > 0)
									{

										//used in order to convert superNode to string
										numberOfFinalsToBePrinted++;

										//have to check if the final in the super node is in the bloom filter

										char *ngram = (char *)malloc(1000);

										memset(ngram, '\0', 1000);

										N_GramMatrix_to_String(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1), &ngram, true);

										//Putting in toAddToPrint the number of bytes to copy in the ngra
										//So we print what we want from the final node

										temp_start = always_on_start;

										if (strlen(ngram) > 0)
											strcat(ngram, " ");

										for (int i = 0, j = 0; j < numberOfFinalsToBePrinted; i++)
										{
											if (i > 0)
												strcat(ngram, " ");

											strncat(ngram, temp_start, (unsigned)abs(sizeArray[i]));

											if (sizeArray[i] > 0)
												j++;

											temp_start += (unsigned)abs(sizeArray[i]);
										}

										if (blF->is_NGram_to_BloomFilter(ngram) == false)
										{

											if (found_N_Gram == true)
											{
												// cout << "|";
												Th_Array->Insert(query_MainCounter, "|");
											}

											//print the NGram
											// cout<<ngram;
											Th_Array->Insert(query_MainCounter, ngram);
											// fflush(stdout);

											pthread_mutex_lock(&heap_mutex);
											heap.InsertHeap(ngram);
											pthread_mutex_unlock(&heap_mutex);
											///////////

											found_N_Gram = true;

											//if NGram not in BloomFilter we add it
											blF->insert_NGram_to_BloomFilter(ngram);
										}

										free(ngram);
									}

									snw_it = snw_it + (unsigned)abs(sizeArray[w]);

									///////////////move on Ngram//////////////////

									space = strchr(ng_it, ' ');
									multSpace = space;

									//take first word of next
									if (space != NULL) //not last word
									{
										do
										{
											multSpace++;
										} while (*multSpace == ' ');

										space = --multSpace;

										ng_it = ++space;

										if (strchr(ng_it, ' ') != NULL)
										{
											freeFirstWord = true;
											if (firstWord != NULL)
											{
												free(firstWord);
												firstWord = NULL;
											}
											takeWord(&firstWord, ng_it);
										}
										else
										{
											freeFirstWord = false;
											firstWord = ng_it;
										}
									}
									else
									{
										// boolFlag = 1;
										firstWord = ng_it;
										freeFirstWord = false;
										break;
										// lastWordFlag = true;
									}

									//////////////////////////////////////
								}
								else
								{
									boolFlag = true;
									break;
								}
							}
							else
							{
								boolFlag = true;
								break;
							}
						}

						insertToMatrix(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1), pToTrieNode, &N_GramMatrixTimesDuplicated,positionToInsertInNGramMatrix);
						positionToInsertInNGramMatrix++;
					}

					current = pToTrieNode;

					/////////move on NGRAM/////////////
					if (boolFlag == false)
					{

						for (int i = 0; i < num_of_words; i++)
						{

							space = strchr(next, ' ');
							multSpace = space;

							// if (space == NULL) //last word
							// next = space;
							if (space != NULL)
							{
								do
								{
									multSpace++;
								} while (*multSpace == ' ');

								space = --multSpace;
								next = ++space;
							}
						}
					}
					else
					{
						//In order to set Iterator again
						// if (firstWord != NULL)
						// 	free(firstWord);
						break;
					}
				}

				//not superNode
				else
				{
					//the word exists in an N-gram
					insertToMatrix(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1), pToTrieNode, &N_GramMatrixTimesDuplicated,positionToInsertInNGramMatrix);
					positionToInsertInNGramMatrix++;
					//check if NGram in BloomFilter so as to avoid duplicates and PRINT
					if (((pToTrieNode->Get_If_Final()) == true))
					{

						if (pToTrieNode->getAdded() > query_id ||
							(pToTrieNode->getDeleted() != -1 &&
							 pToTrieNode->getDeleted() < query_id))
						{
							// if(found_N_Gram == false)
							// Th_Array->Insert(query_MainCounter, "-1");
							// return;
						}
						else
						{

							char *ngram = (char *)malloc(3000 * sizeof(char));
							memset(ngram, '\0', 3000 * sizeof(char));

							N_GramMatrix_to_String(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1), &ngram, true);

							// do not print duplicates
							if (blF->is_NGram_to_BloomFilter(ngram) == false)
							{

								// if(strcmp("january",pToTrieNode->getWord())==0){
								// cout<<"QID : "<<query_id<<" "<<ngram<<" "<<pToTrieNode->getAdded()<<" "<<pToTrieNode->getDeleted()<<endl;
								//
								// }
								if (found_N_Gram == true)
								{
									Th_Array->Insert(query_MainCounter, "|");
								}
								// cout << query_MainCounter<< endl;
								Th_Array->Insert(query_MainCounter, ngram);

								// //HEAP TO DO
								pthread_mutex_lock(&heap_mutex);
								heap.InsertHeap(ngram);
								pthread_mutex_unlock(&heap_mutex);
								////////

								found_N_Gram = true;

								//if NGram not in BloomFilter we add it
								blF->insert_NGram_to_BloomFilter(ngram);
							}

							free(ngram);
						}
					}

					current = pToTrieNode;

					//move on NGRAM/////////////
					space = strchr(next, ' ');
					multSpace = space;

					if (space == NULL) //last word
						next = space;
					else
					{
						do
						{
							multSpace++;
						} while (*multSpace == ' ');

						space = --multSpace;
						next = ++space;
					}
				}
			}
			else
			{
				cleanN_GramMatrix(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1));
				positionToInsertInNGramMatrix=0;
				if (freeFirstWord == true)
				{
					free(firstWord);
					firstWord = NULL;
					freeFirstWord = false;
				}

				break;
			}

			if (space == NULL)
			{
				if (freeFirstWord == true)
				{
					free(firstWord);
					firstWord = NULL;
					freeFirstWord = false;
				}
				break;
			}
			if (freeFirstWord == true)
			{
				free(firstWord);
				firstWord = NULL;
				freeFirstWord = false;
			}
		} while (next != NULL);

		if (freeFirstWord == true)
		{
			free(firstWord);
			freeFirstWord = false;
		}

		next = strchr(iterator, ' ');
		multSpace = next;

		if (next != NULL)
		{
			do
			{
				multSpace++;
			} while (*multSpace == ' ');

			next = multSpace;
			// next++;
		}
		else
			next = iterator;

		current = base->getRoot();

		temp = strchr(iterator, ' ');
		multSpace = temp;

		if (temp != NULL)
		{ //last word
			do
			{
				multSpace++;
			} while (*multSpace == ' ');

			temp = --multSpace;
			temp++;
			iterator = temp;
		}
		else
			iterator = NULL;

		cleanN_GramMatrix(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1));
		positionToInsertInNGramMatrix=0;
	} while (iterator != NULL);

	if (found_N_Gram == false)
	{ // Could not find N_Gram in Trie
		// cout << "-1";
		Th_Array->Insert(query_MainCounter, "-1");
		// fflush(stdout);
	}

	// cout << endl;
	Th_Array->Insert(query_MainCounter, "\n");

	//reset the printed flags in the Trie for the new query
	cleanN_GramMatrix(N_GramMatrix, N_GRAM_MATRIX_SZE * pow(2, N_GramMatrixTimesDuplicated - 1));
	positionToInsertInNGramMatrix=0;
	free(N_GramMatrix);

	//destroy bloomFilter
	delete (blF);
}

//-----------------------------------------------------------------------------------------------------

//----------------FOR THRREADS-----------------------------------------------------------------------

void *Wait_for_Job_and_Execute(void *args)
{
	Queue *Sch_Queue;
	Sch_Queue = Sch->get_Scheduler_Queue();

	if (queryFile.good())
	{

		//Thread runs until the query file is over and still work to be done
		while (true)
		{

			//mutex lock
			pthread_mutex_lock(&sch_mutex);

			//wait on cond var q_empty
			//empty queue or not full queue
			while (Sch_Queue->empty() || Full_Queue == false)
			{

				numberOfThreadsWaiting++;

				if (numberOfThreadsWaiting == THREADS_NUM)
				{
					// pthread_mutex_unlock(&barrier_mutex);
					pthread_cond_signal(&waiting_for_threads);
					// numberOfThreadsWaiting=0;
				}

				// cout<<"Thread: "<< pthread_self()<<" queue size: "<< Sch_Queue->getSize()<<endl;
				pthread_cond_wait(&sch_queue_empty, &sch_mutex);

				// cout<<"AAAAA"<<endl;

				if ((Sch_Queue->empty()) && (queryFile.eof()))
				{
					pthread_mutex_unlock(&sch_mutex);
					pthread_exit(NULL);
				}
			}

			//get job from queue
			// cout<<"JOD BEFORE"<<endl;

			Static_Args *sa = ((Job_Static *)(Sch->get_First_Job()))->get_Static_Args();
			Job_Static sj(sa->getIndex(), sa->getQuery(), sa->getHeap(), sa->get_Job_Id(), sa->get_Job_Id_MainCounter());

			Sch->Remove_Job_From_Sceduler();
			// cout<<"JOD IS "<< sa->get_Job_Id() <<endl;

			//mutex unlock
			pthread_mutex_unlock(&sch_mutex);

			//execute job
			sj.call_Thread_Function(sj.get_Static_Args()->getIndex(), sj.get_Static_Args()->getQuery(), sj.get_Static_Args()->getHeap(), sj.get_Static_Args()->get_Job_Id(), sj.get_Static_Args()->get_Job_Id_MainCounter());

			pthread_mutex_lock(&numberOfThreadsWaiting_mutex);

			countNumberOfThreadsDone++;

			if (countNumberOfThreadsDone == numberOfJobs)
			{
				// pthread_mutex_unlock(&barrier_mutex);
				pthread_cond_signal(&waiting_for_threads);
			}

			pthread_mutex_unlock(&numberOfThreadsWaiting_mutex);
		}
	}

	return NULL;
}

void Create_Jobs_and_Feed_Threads(Index *base, Heap *heap)
{

	//Read from query file
	Queue *queueForAddsDeletes = new Queue();
	string line;
	Trie_Node *root = base->getRoot();
	int countWords = 0;
	char *newText = NULL;
	if (queryFile.good())
	{

		//counter for all lines
		int query_counter = 0;

		//counter for queries only
		int query_MainCounter = 0;

		// pthread_mutex_lock(&barrier_mutex);

		while (getline(queryFile, line))
		{

			// GetLineInfo(line, countWords, words);

			char typeOfQuery = line.at(0);
			if (typeOfQuery == 'A')
			{
				query_counter++;
				queueForAddsDeletes->push(countWords, line, 1, 'A', query_counter);
			}
			else if (typeOfQuery == 'D')
			{
				query_counter++;
				//1 here doesnt matter
				queueForAddsDeletes->push(countWords, line, 1, 'D', query_counter);
			}
			else if (typeOfQuery == 'Q')
			{

				char *text = NULL;

				text = (char *)malloc(line.length() + 1);
				convertStringToChar(line, text);

				newText = text + 2; //pure text

				//Create Static Job....
				query_counter++;
				query_MainCounter++;
				// cout << "we are the same " << query_MainCounter << " " << query_counter <<endl;

				// Static_Args args(base, newText, heap, query_counter);
				// Job_Static job(&args);

				//...and subit it to Scheduler

				Sch->get_Scheduler_Queue()->push(base, newText, heap, query_counter, query_MainCounter);

				// delete args;
				// delete job;
				free(text);
			}
			else if (typeOfQuery == 'F')
			{
				query_counter++;
				int topK = 0;
				countWords = 1;
				if (line.size() > 1)
				{
					string number = line.substr(2, '\n');
					if (number.size() != 0)
					{
						countWords = 2;
						topK = atoi(number.c_str());
					}
					else
					{
						topK = 0;
						countWords = 1;
					}
				}
				//if we have something in queueForAddsDeletes
				//dynamic tries
				// cout << "queue size is " << queueForAddsDeletes->empty() << endl;
				while (queueForAddsDeletes->empty() == false)
				{
					// Static_Args *sa = ((Job_Static *)(queueForAddsDeletes->get_First_Job()))->get_Static_Args();
					// Job_Static sj(sa->getCountWords(),sa->getWords(),);

					Job_Static *temp = (Job_Static *)queueForAddsDeletes->front();
					// cout << "we are in loop \n" << endl;
					// cout <<"Queue size is " << queueForAddsDeletes->getSize() << endl;
					// cout <<"Job id in queue for add and delete is " << temp->get_Static_Args()->get_Job_Id() << endl;
					//execute job
					if (temp->get_Static_Args()->getType() == 'A')
					{
						// temp->call_Add_Function(root);
						root->Add(temp->get_Static_Args()->getCountWords(),
								  temp->get_Static_Args()->getWords(),
								  temp->get_Static_Args()->getChoice(),
								  temp->get_Static_Args()->get_Job_Id());
					}
					else if (temp->get_Static_Args()->getType() == 'D')
					{
						// temp->call_Delete_Function(root);
						root->Remove(temp->get_Static_Args()->getWords(),
									 temp->get_Static_Args()->getCountWords(),
									 temp->get_Static_Args()->get_Job_Id());
					}

					queueForAddsDeletes->pop();
				}

				numberOfJobs = Sch->get_Scheduler_Queue()->getSize();
				// cout<<"AAAAA : "<<numberOfJobs << " " << query_MainCounter<<endl;
				// cout << <query_MainCounter << endl;
				Th_Array = new Parallel_Printing_Array(query_MainCounter);

				//call scheduler to...
				//give jobs to threads from thread pool
				pthread_mutex_lock(&sch_mutex);
				Full_Queue = true;
				pthread_mutex_unlock(&sch_mutex);

				// cout<<"BROADCAST"<<endl;
				pthread_cond_broadcast(&sch_queue_empty);

				//we lock the mutex cause we need it to wait
				//for the last thread to unlock it
				//-----first locked in Main ----
				pthread_mutex_lock(&barrier_mutex);
				while (countNumberOfThreadsDone < numberOfJobs)
				{
					pthread_cond_wait(&waiting_for_threads, &barrier_mutex);
				}
				// cout << "we are locked"<<endl;

				countNumberOfThreadsDone = 0;
				pthread_mutex_unlock(&barrier_mutex);
				// cout <<"main unlocked" << endl;
				//print the helper struct
				Th_Array->Print();
				//and clean it to be constructed again
				Th_Array->Clear();
				delete Th_Array;

				pthread_mutex_lock(&sch_mutex);
				Full_Queue = false;
				pthread_mutex_unlock(&sch_mutex);

				pthread_mutex_lock(&heap_mutex);
				if (countWords == 2)
				{

					if (heap->isEmpty() == false)
					{
						//QuickSort
						Node node = heap->getNode();
						int *tempF = node.getFrequencies();
						char **tempStrings = node.getStrings();
						// sstime = ((double)clock()) / CLOCKS_PER_SEC;
						quicksort_body(topK, tempF, tempStrings, heap->getCurrentCapacity());
						//end of QUICKSORT

						//Print Heap
						heap->print(topK);
					}
				}

				heap->reset();
				pthread_mutex_unlock(&heap_mutex);
				// query_counter = 0;
				query_MainCounter = 0;
				////
			}

			// FreeInfo(words, countWords);
			countWords = 0;
		}
		//reached EOF .Call threads to exit
		pthread_cond_broadcast(&sch_queue_empty);
	}

	delete (queueForAddsDeletes);
}

//---------------------------------------------------------------------------------------------------
