#ifndef __guard4__
#define __guard4__

void search_Compressed_Trie(Index *, char *, Heap &, int , int );

#include "HelperFunctions.h"
//bytes in the threads structure 
#define PREALLOCATED_SPACE 100000

class Static_Args{

	//the arguments of the search function
	Index *base;
	char *query;
	Heap *heap;
	int job_id;


	//for add and delete only
	int choice;
	char type;
	int countWords;
	char **words;
	int qc_Main;
public:


	//constructor for queries
	Static_Args(Index *&b, 
				char *&s, 
				Heap *&h,
				int qc,
				int qc_Main)
	{
		base = b;

		query = (char *)malloc(strlen(s)+1);
		memset(query,'\0',strlen(s)+1);
		strcpy(query, s);

		heap = h;
		job_id = qc;
		this->qc_Main= qc_Main;
		this->type = -1;
		this->choice= 0;
		this->countWords = -1;
		this->words = NULL;
	}

	//constructor for Add
	Static_Args(int countWords,string line,int choice,char type,int qc) {

		this->words = NULL;
		qc_Main = 0;
		GetLineInfo(line, countWords, words);
		
		job_id = qc;
		this->qc_Main = 0;
		this->countWords = countWords;
		this->choice = choice;
		this->type = type;
		query = NULL;
	}

	//constructor for Delete
	Static_Args (int countWords,string line,char type,int qc) {
		this->words = NULL;
		qc_Main=0;
		GetLineInfo(line, countWords, words);

		job_id = qc;
		this->qc_Main = 0;
		this->countWords = countWords;
		this->choice = choice;
		this->type = type;	
		query = NULL;
	}


	~Static_Args(){
		if (query != NULL)
			free(query);

		if(words != NULL)
			FreeInfo(words, countWords);

	}

	Index *&getIndex() {
		return base;
	}

	int get_Job_Id_MainCounter(){
		return qc_Main;
	}

	char *&getQuery() {
		return query;
	}

	char getType () {
		return type;
	}

	int getChoice() {
		return choice;
	}

	int getCountWords() {
		return countWords;
	}

	char **getWords() {
		return words;
	}


	Heap *&getHeap() {
		return heap;
	}

	int get_Job_Id(){
		return job_id;
	}
};


class Job_Static{

	//a pointer to the static Trie function
	//the thread is going to call this function for search	
	void (*thread_Work)(Index *, char *, Heap &, int, int);
	//the arguments of the srach function to be executed by the thread
	Static_Args *args;

public:
	Job_Static(Index *&b, 
				char *s, 
				Heap *&h,
				int qc,
				int qc_Main){


		thread_Work = search_Compressed_Trie;
		args = new Static_Args(b,s,h,qc,qc_Main);
	}
	~Job_Static(){
		delete(args);
	};

	Job_Static(int countWords,string line,int choice,char type,int qc)
	{
		args = new Static_Args(countWords,line,choice,type,qc);

	}

	Job_Static(int countWords,string line,char type,int query_counter)
	{
		args = new Static_Args(countWords,line,type,query_counter);
	}

	Static_Args *&get_Static_Args() 
	{
		return args;
	}


	void call_Thread_Function(Index *I, char *q, Heap *h, int query_id,int query_counterMain){

		(*thread_Work)(I,q,*h,query_id,query_counterMain);
	}
	
};



typedef struct Parallel_Printing_Array{

	char **Matrix;
	int *Times_Realloced;
	int Num_of_Queries;

	Parallel_Printing_Array(int n){

		Num_of_Queries = n;
		// cout<<"BBBBBB : "<<Num_of_Queries<<endl;


		Matrix = (char **)malloc(Num_of_Queries*sizeof(char *));
		memset(Matrix,(int)NULL, Num_of_Queries*sizeof(char *));
		
		Times_Realloced = (int *)malloc(Num_of_Queries*sizeof(int));
		memset(Times_Realloced, 1, Num_of_Queries*sizeof(int));

		for(int i=0; i<Num_of_Queries; i++){

			Matrix[i] = (char *)malloc(PREALLOCATED_SPACE*sizeof(char));
			memset(Matrix[i], '\0', PREALLOCATED_SPACE*sizeof(char));
		}
	}

	void Insert(int job_id, const char *result){
		// cout << "JOB ID IS " << job_id << endl;
		int current_len = strlen(Matrix[job_id - 1]);
		int new_len = current_len+strlen(result);



		if(new_len >= (Times_Realloced[job_id - 1])*PREALLOCATED_SPACE){

			Times_Realloced[job_id-1]++;

			Matrix[job_id-1] = (char *)realloc(Matrix[job_id-1], PREALLOCATED_SPACE*(Times_Realloced[job_id-1]));

			char *temp = NULL;
			temp = Matrix[job_id-1] + PREALLOCATED_SPACE*(Times_Realloced[job_id-1]-1);
			memset(temp, '\0', PREALLOCATED_SPACE);
		}
		
		strcat(Matrix[job_id - 1],result);

		// if(strcmp(result , "january")==0)
			// cout<<"REALLOC "<<Matrix[job_id - 1]<<endl;

	}


	void Print(){

		for(int i=0; i < Num_of_Queries; i++){

			cout<<Matrix[i];
			fflush(stdout);
		}
	}

	void Clear(){

		for(int i=0; i < Num_of_Queries; i++){

			if(Matrix[i] != NULL)
				free(Matrix[i]);
			Matrix[i] = NULL;
		}

		if(Matrix != NULL)
			free(Matrix);
		Matrix = NULL;

		if(Times_Realloced != NULL)
			free(Times_Realloced);	
		Times_Realloced = NULL;	
	}

}Parallel_Printing_Array;

#endif