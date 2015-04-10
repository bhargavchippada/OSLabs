#include <iostream>
#include <vector>
#include <string>
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

struct sThread
{
	string name;
};

struct mThread
{

};