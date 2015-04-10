#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
using namespace std;

pthread_mutex_t flightMutex[10];
pthread_t threads[5];

struct flight
{
	int name;
	int maxSeatCount;
	int availSeatCount;
	flight(int n, int maxSeatCount)
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
			pthread_mutex_lock (&flightMutex[name]);
			availSeatCount--;
			pthread_mutex_unlock (&flightMutex[name]);
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
			pthread_mutex_lock (&flightMutex[name]);
			availSeatCount++;
			pthread_mutex_unlock (&flightMutex[name]);
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

struct query{
	int actionNo;
	int flightNo;
	query(int ano, int fno){
		actionNo = ano;
		flightNo = fno;
	}
};

vector<query> queries;