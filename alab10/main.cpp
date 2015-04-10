#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

struct flight
{
	int name;
	int maxSeatCount;
	int availSeatCount;

	void init(int n, int maxSC)
	{
		name = n;
		availSeatCount = maxSC;
		maxSeatCount = maxSC;
	}

	void status()
	{
		cout<<"Number of seats available on flight: "<<name<<" are "<<availSeatCount<<endl;
	}

	bool book()
	{
		if(availSeatCount == 0)
		{
			cout<<"All tickets are booked on flight: "<<name<<endl;
			return false;
		}
		else
		{
			cout<<"Booked a ticket on flight: "<<name<<endl;
			availSeatCount--;
			return true;
		}
	}
	
	bool cancel()
	{
		if (availSeatCount == maxSeatCount)
		{
			cout<<"Cannot cancel a ticket on an already empty flight: "<<name<<endl;
			return false;
		}
		else
		{
			cout<<"cancelled a ticket on flight: "<<name<<endl;
			availSeatCount++;
			return true;
		}
	}
};

flight flights[10];
string shared_data = "";
pthread_mutex_t mutexShared,mutexNum,mutexFlight[10];
pthread_cond_t slave,master,master1;
int num_blocked = 5;

void *masterThreadFunc(void* data)
{
  ifstream infile("transactions.txt");
  string line,arg1,arg2;

  while(getline(infile,line))
    {
      istringstream iss(line);
      iss >> arg1;
      if(arg1 == "END")
		{
		  cout << "End of queries reached" << endl;
		  return NULL;
		}
	      else 
		{
		  pthread_mutex_lock(&mutexShared);
		  while(num_blocked == 0)pthread_cond_wait(&master,&mutexShared);
		  shared_data = line;
		  pthread_cond_signal(&slave);
		  num_blocked--;
		  pthread_cond_wait(&master1,&mutexShared);
		  pthread_mutex_unlock(&mutexShared);
		}
    }

   return NULL;
}

void* slaveThreadFunc(void* data)
 {
   srand (time(NULL));
   while(1)
     {
       pthread_mutex_lock(&mutexShared);
       pthread_cond_wait(&slave,&mutexShared);

       cout << "unblock thread : " << (long long) data << " to execute: "<< shared_data << endl;

       istringstream iss(shared_data);
       string arg1,arg2;
       iss >> arg1 >> arg2;
       //cout << "shared "<<shared_data << endl;

       pthread_cond_signal(&master1);

       if ( arg1 == "END")
		 {
		   return NULL;
		 }

       pthread_mutex_unlock(&mutexShared);

       
       pthread_mutex_lock(&mutexFlight[stoi(arg2)]);
	   if( arg1 == "status" )
		 {
		   flights[stoi(arg2)].status();
		 }
	       else if(arg1 == "book")
		 {
		   flights[stoi(arg2)].book();
		 }
	       else if(arg1 == "cancel")
		 {
		   flights[stoi(arg2)].cancel();
		 }
	       pthread_mutex_unlock(&mutexFlight[stoi(arg2)]);
		
       usleep(rand()%100+100);

       pthread_mutex_lock(&mutexShared);
       cout << "blocking thread : " << (long long) data << endl;
       num_blocked++;
       pthread_cond_signal(&master);
       pthread_mutex_unlock(&mutexShared);
 
     } 
  return NULL;
}


int main()
{
	flights[0].init(0, 10);
	flights[1].init(1, 5);
	flights[2].init(2, 5);
	flights[3].init(3, 11);
	flights[4].init(4, 7);
	flights[5].init(5, 8);
	flights[6].init(6, 10);
	flights[7].init(7, 2);
	flights[8].init(8, 23);
	flights[9].init(9, 12);

	pthread_t mThread,sThread[5];
	pthread_create(&mThread,NULL,masterThreadFunc,NULL);
	pthread_mutex_init(&mutexShared, NULL);
  	pthread_mutex_init(&mutexNum, NULL);
  	pthread_cond_init (&slave, NULL);
  	pthread_cond_init (&master, NULL);
  	pthread_cond_init (&master1, NULL);

  	for(int i = 0;i<10;i++)
    {
      pthread_mutex_init(&mutexFlight[i], NULL);
    }

    for(long long i = 0;i<5;i++)
    {
      pthread_create(&sThread[i],NULL,slaveThreadFunc,(void*) i);
    }

  	pthread_join(mThread,NULL);
}
