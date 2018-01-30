/*

	The bit vector is created as an array of integers but its functionallity is implemented on bit level.

	The reason that an array of integers in used instead of another data type is that he standard (C99 6.2.5p5) says:

		- "A "plain" int object has the natural size suggested by the architecture of the execution environment."

		...so in most cases is more efficient.

*/

#ifndef __guard15__
#define __guard15__

# include <cstring>
# include <cstdio>
# include <iostream>
# include <unistd.h>
# include <cstdlib>
# include <cmath>

# include "murmur3.h"

using namespace std;

//THE NUMBER OF BITS OF THE BIT VECTOR
#define expectedElements 5000

//THE NUMBER OF BITS OF INTEGER REPRESENTATION IN THE SYSTEM
const size_t int_Bits = sizeof(int)*8;



class BloomFilter{

	//number of elements inserted
	int numberOfElementsInserted;

	//bit vector size
	int bitVectorSize;
	
	//number of hash functions
	int numberOfHashFunctions;

	//false positive propability
	float falsePositiveProbability;

	//false -> little endian , true -> big endian
	bool endian; 

	//the bit vecto of the filter
	int *bitVector;

	public:

		BloomFilter(bool endval, float pval);

		~BloomFilter(){

			if(bitVector != NULL)
				free(bitVector);
		}


		int *getBitVector(){

			return bitVector;
		}

		//assumes little endian
		void printBits(void const * const ptr,FILE *fp)
		{
		    fwrite(ptr,bitVectorSize/8,1,fp);
		}
		
		void setBitInVector(int k);
		void clearBitInVector(int k);
		int checkBitinVector(int k);

		void insert_NGram_to_BloomFilter(const char *ngram);
		bool is_NGram_to_BloomFilter(const char *ngram);
		void reset_BloomFilter();
};

#endif