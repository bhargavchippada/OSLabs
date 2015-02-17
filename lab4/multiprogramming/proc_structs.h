#include <vector>
using namespace std;

struct process_phase {
	int itrs;	//number of iterations
	int cpu_b;	//cpu burst time
	int io_b;	//IO burst time
	void dec(){
		itrs--;
	}
};

struct process {
	int pid;
	int start_priority;
	int admission;
	int curr_ptr;
	vector<process_phase> phases;

	process(){
		curr_ptr = 0;
	}

	pair <int, int> getWork()
	{
		//cout<<"Phase: "<<" "<<curr_ptr<<" "<<phases[curr_ptr].itrs<<" "<<phases[curr_ptr].cpu_b<<" "<<phases[curr_ptr].io_b<<endl;
		pair<int,int> p;
		if (curr_ptr >= phases.size())
		{
			p.first = -1;
			p.second = -1;
		}
		else
		{
			if (phases[curr_ptr].itrs == 0)
			{
				curr_ptr++;
				return getWork();
			}
			p.first = phases[curr_ptr].cpu_b;
			p.second = phases[curr_ptr].io_b;
			phases[curr_ptr].dec();
		}
		return p;
	}
};
