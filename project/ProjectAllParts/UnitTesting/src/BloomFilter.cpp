# include "BloomFilter.h"


BloomFilter::BloomFilter(bool endval, float pval){
		

	endian = endval;
	
	int nval = expectedElements;

	numberOfElementsInserted = nval;
	falsePositiveProbability = pval;

	// bitVectorSize = -(numberOfElementsInserted*log(falsePositiveProbability))/pow(log(2),2);
	bitVectorSize = 32*numberOfElementsInserted;

	numberOfHashFunctions = (bitVectorSize/numberOfElementsInserted)*log(2);

	//create the bitVector (+1 in malloc for the remaoinder bits of the division...otherwise seg fault)
	bitVector = (int *)malloc(((bitVectorSize/int_Bits)+1)*sizeof(int));

	//initialize the BitVector with zeroes
	memset(bitVector, 0, (bitVectorSize/int_Bits)*sizeof(int));

	// cout<<"Blomm created with n: "<<numberOfElementsInserted<<" m: "<<bitVectorSize<<" k: "<<numberOfHashFunctions<<endl;
}


//Set k bit in vector to 1.
void BloomFilter::setBitInVector(int k){

	//start from zero coutn...the user input start from 1
	// k = k-1;

	int i = k/int_Bits;            // i = array index (use: A[i])
	int pos = k%int_Bits;          // pos = bit position in A[i]

		// cout<<"pos is "<<pos<<endl;

	//big endian architecture
	if(endian){

		unsigned int mask = 1;   // mask = 0000.....00001

		mask = mask << (sizeof(int)*8-1); // mask = 10000....000000
  		mask = mask >> pos;      // mask = 0000...010...000   (shifted k positions)

  		bitVector[i] = bitVector[i] | mask;      // Set the bit at the k-th position in bitVector[i]				
	}
	//little endian architecture
	else{

		unsigned mask = 1;   // mask = 00000001.....00000000
		int a;
		int shift;

		if ((pos+1)%8 != 0){

			a = ((pos+1)/8) + 1;
			shift = 8*a - (pos+1)%8;
		}
		else{

			a = (pos+1)/8;

			if(pos+1 == 8)
				shift = 0;
			else		
				shift = 8 * (a-1);
		}

		mask = mask << shift;
		
		// printBits(sizeof(mask),&mask,fp);
		// cout<<"k "<<k+1<<" pos "<<pos<<" a "<<a<<" shift "<< shift<<endl;
		// cout<<" i is"<< i <<" and bvs is "<<(bitVectorSize/int_Bits)<<endl;
		bitVector[i] = bitVector[i] | mask;      // Set the bit at the k-th position in bitVector[i]
		
	}
}

//Set k bit in Vector to 0.
void BloomFilter::clearBitInVector(int k){

	//start from zero coutn...the user input start from 1
	// k = k-1;

	int i = k/int_Bits;            // i = array index (use: A[i])
		int pos = k%int_Bits;          // pos = bit position in A[i]

		// cout<<"pos is "<<pos<<endl;

	//big endian architecture
	if(endian){

		unsigned int mask = 1;   // mask = 0000.....00001

		mask = mask << (sizeof(int)*8-1); // mask = 10000....000000
  		mask = mask >> pos;      // mask = 0000...010...000   (shifted k positions)

  		bitVector[i] = bitVector[i] & mask;     // RESET the bit at the k-th position in bitVector[i]				
	}
	//little endian architecture
	else{

		unsigned mask = 1;   // mask = 00000001.....00000000
		int a;
		int shift;

		if ((pos+1)%8 != 0){

			a = ((pos+1)/8) + 1;
			shift = 8*a - (pos+1)%8;
		}
		else{

			a = (pos+1)/8;

			if(pos+1 == 8)
				shift = 0;
			else		
				shift = 8 * (a-1);
		}

		mask = mask << shift;
		
		// printBits(sizeof(mask),&mask,fp);
		// cout<<"k "<<k+1<<" pos "<<pos<<" a "<<a<<" shift "<< shift<<endl;

		bitVector[i] = bitVector[i] & mask;     // RESET the bit at the k-th position in bitVector[i]
	}	
}


//checks the k bit in bitVector and returns its value (0 or 1)
int BloomFilter::checkBitinVector(int k){

	//start from zero coutn...the user input start from 1
	// k = k-1;

	int i = k/int_Bits;            // i = array index (use: A[i])
		int pos = k%int_Bits;          // pos = bit position in A[i]

		// cout<<"pos is "<<pos<<endl;

	//big endian architecture
	if(endian){

		unsigned int mask = 1;   // mask = 0000.....00001

		mask = mask << (sizeof(int)*8-1); // mask = 10000....000000
  		mask = mask >> pos;      // mask = 0000...010...000   (shifted k positions)

  		bitVector[i] = bitVector[i] | mask;      // Set the bit at the k-th position in bitVector[i]

  		return 0;				
	}
	//little endian architecture
	else{

		unsigned mask = 1;   // mask = 00000001.....00000000
		int a;
		int shift;

		if ((pos+1)%8 != 0){

			a = ((pos+1)/8) + 1;
			shift = 8*a - (pos+1)%8;
		}
		else{

			a = (pos+1)/8;

			if(pos+1 == 8)
				shift = 0;
			else		
				shift = 8 * (a-1);
		}

		//creating the appropraite mask 0000010000.....00000
		mask = mask << shift;
		
		//and doing logical and
		int check;
		
		check = bitVector[i] & mask;      

		//return the value of the bit in the bitVector
		if(check == 0)
			return 0;
		else
			return 1;
	}
}


void BloomFilter::insert_NGram_to_BloomFilter(const char *ngram){

	//-----------------------------------------------------------------------
	/*
		Implemention according to Adam Kirsch and Michael Mitzenmacher paper. 

		Specifically, only two hash functions are necessary to effectively implement a Bloom filter without any loss in
		the asymptotic false positive probability. This leads to less computation and potentially less need for
		randomness in practice.
	*/
	//-----------------------------------------------------------------------

	//each table position is the value of one of the two hash functions
	uint64_t murmur_Hash[2];
	uint64_t h1,h2;
	
	int gHash;

	MurmurHash3_x64_128(ngram, strlen(ngram), 0, murmur_Hash);

	h1 = murmur_Hash[0];
	h2 = murmur_Hash[1];

	/*
		Calculate k-values accroding to extended double hashing

		like g(x) = (h1(x) + i*h2(x) + f(i)) mod m
		
		f(i) is an arbitary function so long as it is drawn independently of the two random hash
		
		functions h1 and h2 .
	*/

	// f(i) in this case is f(i) = i^3

	for(unsigned i=1;i<=(unsigned)numberOfHashFunctions;i++){

		gHash = (h1 + i*h2 + (unsigned)pow(i,3)) % bitVectorSize;
		// cout<<"gHash "<<gHash<<" and bvs in bits "<<bitVectorSize<<endl;

		setBitInVector(gHash);
	}

}


//Checking if an NGram is in the BloomFilter
//and returns true or false
bool BloomFilter::is_NGram_to_BloomFilter(const char *ngram){

	//each table position is the value of one of the two hash functions
	uint64_t murmur_Hash[2];
	uint64_t h1,h2;
	uint64_t gHash;

	MurmurHash3_x64_128(ngram, strlen(ngram), 0, murmur_Hash);

	h1 = murmur_Hash[0];
	h2 = murmur_Hash[1];

	/*
		Calculate k-values accroding to extended double hashing

		like g(x) = (h1(x) + i*h2(x) + f(i)) mod m
		
		f(i) is an arbitary function so long as it is drawn independently of the two random hash
		
		functions h1 and h2 .
	*/

	// f(i) in this case is f(i) = i^3

	for(unsigned i=1;i<=(unsigned)numberOfHashFunctions;i++){

		gHash = (h1 + i*h2 + (unsigned)pow(i,3)) % bitVectorSize;
		// cout<<"gHash "<<gHash<<endl;

		//check the value of the bit in bitVector
		//if one bit is set to 0 then the ngram is NOT in the filter
		//else it probably is
		if(checkBitinVector(gHash) == 0){
			return false;
		}

	}

	return true;	
}

void BloomFilter::reset_BloomFilter(){

	memset(bitVector,0,((bitVectorSize/int_Bits)+1)*sizeof(int));
}