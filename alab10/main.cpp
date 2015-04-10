#include "main.h"

extern vector<flight*> flights;
extern mThread *batman;
extern pthread_mutex_t flightMutex[10];
extern vector<query> queries;

int main()
{
	flights.push_back(new flight(0, 10));
	flights.push_back(new flight(1, 5));
	flights.push_back(new flight(2, 6));
	flights.push_back(new flight(3, 11));
	flights.push_back(new flight(4, 7));
	flights.push_back(new flight(5, 8));
	flights.push_back(new flight(6, 10));
	flights.push_back(new flight(7, 2));
	flights.push_back(new flight(8, 23));
	flights.push_back(new flight(9, 12));

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
