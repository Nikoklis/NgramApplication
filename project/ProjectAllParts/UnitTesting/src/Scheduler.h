#ifndef __guard5__
#define __guard5__

#include "Jobs.h"
#define INIT_SIZE 50


class Queue
{
  public:
	void **Sch_Queue;
	int lastElement;
	int timesRealloced;
	int head;
	int size;

	Queue()
	{
		lastElement = 0;
		head = 0;
		size = 0;
		timesRealloced = 0;
		Sch_Queue = (void **)malloc(sizeof(void *) * INIT_SIZE);
		memset(Sch_Queue,(int)NULL,sizeof(void *) * INIT_SIZE);
	}

	//push for Queries
	void push(Index *&b, 
				char *s, 
				Heap *&h,
				int qc,
				int qc_Main)
	{
		if (lastElement == (INIT_SIZE * (timesRealloced + 1)))
		{
			//we realloc our queue
			timesRealloced++;
			Sch_Queue = (void **)realloc(Sch_Queue, 2 * sizeof(void *) * INIT_SIZE * timesRealloced);
			void *temp = NULL;
			temp = Sch_Queue + INIT_SIZE*(timesRealloced);
			memset(temp, (int)NULL, INIT_SIZE);
		}
		// Sch_Queue[lastElement] = (Job_Static *)malloc(sizeof(Job_Static));
		Sch_Queue[lastElement] = (Job_Static *)new Job_Static(b,s,h,qc,qc_Main);
		// memcpy(Sch_Queue[lastElement], job, sizeof(Job_Static));
		lastElement++;
		size++;
	}
	////

	//push for Add and Delete
	void push(int countWords,string line,int choice,char type,int qc) {
		if (lastElement == (INIT_SIZE * (timesRealloced + 1)))
		{
			//we realloc our queue
			timesRealloced++;
			Sch_Queue = (void **)realloc(Sch_Queue, 2 * sizeof(void *) * INIT_SIZE * timesRealloced);
			void *temp = NULL;
			temp = Sch_Queue + INIT_SIZE*(timesRealloced);
			memset(temp, int(NULL), INIT_SIZE);
		}

		if (type == 'A')
		{
			Sch_Queue[lastElement] = (Job_Static *)new Job_Static(countWords,line,choice,type,qc);
		}
		else if(type == 'D')
		{
			Sch_Queue[lastElement] = (Job_Static *)new Job_Static(countWords,line,type,qc);
		}
		else
			cout <<"cancer" << endl;
		
		lastElement++;
		size++;
		// cout<<"Inserting : "<<lastElement<<endl;
		// cout<<"size in push "<<size<<endl;
	}
	////

	void pop()
	{
		if (Sch_Queue[head] != NULL)
		{
			// cout << " head is " << head << endl;
			delete((Job_Static *)Sch_Queue[head]);
			// Sch_Queue[head] = NULL;
			// cout<<"Deleting : "<<head<<endl;
			// cout<<"size in pop "<<size<<endl;
			
			head++;
			size--;
		}
		// cout << "general pop" << endl;
		if (head == lastElement)
		{
			head = 0;
			lastElement = 0;
			size = 0;
		}
	}

	void *front()
	{
		return (Sch_Queue[head]);
	}

	bool empty()
	{
		return (size == 0);
	}

	int getSize() {
		return size;
	}

	~Queue()
	{
		free(Sch_Queue);
	}
};

class Scheduler
{

	//The scheduler Queue holding the Jobs
	//to be executed by threads
	// std::queue<void *> *Sch_Queue;
	// void **Sch_Queue;
	// int lastElement;
	// int timesRealloced;
	// int head;
	// int size;
	Queue *Sch_Queue;


  public:
	Scheduler()
	{
		Sch_Queue = new Queue();
	}
	~Scheduler()
	{
		delete (Sch_Queue);
	}

	Queue *get_Scheduler_Queue()
	{
		return Sch_Queue;
	}

	bool empty()
	{
		return (Sch_Queue->empty());
	}

	// void Submit_Job_To_Scheduler(Index *&b, 
	// 							char *s, 
	// 							Heap *&h,
	// 							int qc)
	// {
	// 	Sch_Queue->push(b,s,h,qc);
	// }

	void Remove_Job_From_Sceduler()
	{
		Sch_Queue->pop();
	}

	void *get_First_Job()
	{
		if (!(Sch_Queue->empty()))
			return (Sch_Queue->front());
		return NULL;
	}
};

#endif
