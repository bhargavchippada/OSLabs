#include "main.h"

extern vector<flight*> flights;
extern mThread *batman;
extern pthread_mutex_t flightMutex[10];
extern vector<query> queries;

int main()
{
	flights.push_back(new flight("Flight0", 10));
	flights.push_back(new flight("Flight1", 5));
	flights.push_back(new flight("Flight2", 6));
	flights.push_back(new flight("Flight3", 11));
	flights.push_back(new flight("Flight4", 7));
	flights.push_back(new flight("Flight5", 8));
	flights.push_back(new flight("Flight6", 10));
	flights.push_back(new flight("Flight7", 2));
	flights.push_back(new flight("Flight8", 23));
	flights.push_back(new flight("Flight9", 12));
	for(int i=0; i<10; i++){
		//flightMutex[i]=PTHREAD_MUTEX_INITIALIZER;
	}

	queries.push_back(*(new query(0,1)));
	queries.push_back(*(new query(2,3)));
	queries.push_back(*(new query(1,2)));
	queries.push_back(*(new query(2,5)));
	queries.push_back(*(new query(1,4)));
	queries.push_back(*(new query(1,5)));
	queries.push_back(*(new query(1,6)));
	queries.push_back(*(new query(1,1)));
	queries.push_back(*(new query(1,2)));
	queries.push_back(*(new query(0,2)));
	queries.push_back(*(new query(2,1)));
	queries.push_back(*(new query(2,1)));

}
