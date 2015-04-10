#include "main.h"
vector<flight> flights;

int main()
{
	flight *f0 = new flight("Flight0", 10);
	flight *f1 = new flight("Flight1", 5);
	flight *f2 = new flight("Flight2", 6);
	flight *f3 = new flight("Flight3", 11);
	flight *f4 = new flight("Flight4", 7);
	flight *f5 = new flight("Flight5", 8);
	flight *f6 = new flight("Flight6", 10);
	flight *f7 = new flight("Flight7", 2);
	flight *f8 = new flight("Flight8", 23);
	flight *f9 = new flight("Flight9", 12);
	flights.push_back(*f0);
	flights.push_back(*f1);
	flights.push_back(*f2);
	flights.push_back(*f3);
	flights.push_back(*f4);
	flights.push_back(*f5);
	flights.push_back(*f6);
	flights.push_back(*f7);
	flights.push_back(*f8);
	flights.push_back(*f9);
}
