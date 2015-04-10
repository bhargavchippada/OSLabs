#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
using namespace std;

int qNo;

struct flight
{
	string name;
	int maxSeatCount;
	int availSeatCount;
	flight(string n, int maxSeatCount)
	{
		name = n;
		availSeatCount = maxSeatCount;
	}
	bool checkIfAvail()
	{
		if (availSeatCount == 0)
		{
			return false;
		}
		return true;
	}
	bool book()
	{
		if(!checkIfAvail())
		{
			return false;
		}
		else
		{
			availSeatCount--;
			return true;
		}
	}
	bool cancel()
	{
		if (availSeatCount == maxSeatCount)
		{
			return false;
		}
		else
		{
			availSeatCount++;
			return true;
		}
	}
};

vector<flight*> flights;

void *sThread(void *tname)
{
	long name = *(long*)tname;
	int q;
	int flightNo;
	//Read from file
	if (q == 1)
	{
		if (flights[flightNo]->book()) 
		{
			cout<< "Flight " << flightNo << " has been booked using thread " << name << ".\n";
		}
		else
		{
			cout << "Booking failed on flight " << flightNo << " using thread " << name << ". Please try another flight.\n";
		}
	}
	else if (q == 0)
	{
		if (flights[flightNo]->checkIfAvail())
		{
			cout << "Seats are available on flight " << flightNo << " (checked by thread " << name << ").\n";
		}
		else
		{
			cout << "We're sorry, but seats aren't available on flight " << flightNo << " (checked by thread " << name << ").\n";
		}
	}
	else if (q == 2)
	{
		if (flights[flightNo]->cancel()) 
		{
			cout << "Your seat has successfully been cancelled on flight " << flightNo << " by thread " << name << ".\n";
		}
		else
		{
			cout << "Whoops! No seat was cancelled on flight " << flightNo << ". Please check again.\n";
		}
	}
	else cout << "Invlid Op!\n";
}

void *executeSlaves(void *mId)
{

}

struct mThread
{
	pthread_t master;
	pthread_t threads[5];
	queue<int> jobList;
	int slaveCounter;
	mThread()
	{
		int mId = 0;
		pthread_create(&master, NULL, executeSlaves, (void *)&mId);
		for (long i = 0; i < 5; ++i)
		{
			pthread_create(&threads[i], NULL, sThread, (void *)&i);
		}
	}
};

mThread *batman = new mThread();
