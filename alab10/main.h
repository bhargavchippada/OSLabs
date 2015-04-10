#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
using namespace std;

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

struct sThread
{
	string name;
	sThread(string threadname)
	{
		name = threadname;
	}
	void bookFlight(int flightNo)
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
	void checkStatus(int flightNo)
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
	void cancelFlight(int flightNo)
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
};

struct mThread
{
	vector<sThread*> slaves;
	queue<int> jobList;
	int slaveCounter;
	mThread()
	{
		slaves.push_back(new sThread("IronMan"));
		slaves.push_back(new sThread("Hulk"));
		slaves.push_back(new sThread("BlackWidow"));
		slaves.push_back(new sThread("Thor"));
		slaves.push_back(new sThread("Loki"));
		slaveCounter = 5;
	}
	int freeSlave()
	{
		//return a random free slave
		return 0;
	}
	void execQuery(int query, int slaveNo)
	{

	}
	void getQuery(int query)
	{
		if (slaveCounter == 5)
		{
			jobList.push(query);
		}
		else
		{
			execQuery(query, freeSlave());
			slaveCounter++;
		}
	}
};

mThread *batman = new mThread();
