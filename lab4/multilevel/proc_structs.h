#include <vector>
using namespace std;

struct scheduling_level {
	int level_no;
	int priority;
	int time_slice;	//either some integer or N
	int level_count;

	scheduling_level(){
		level_count = 0;
	}
};

struct scheduler {
	int no_of_levels;
	vector<scheduling_level> levels_arr;

	scheduler(){
		no_of_levels = 0;
	}
	int getTimeSlice(int i){
		return levels_arr[i].time_slice;
	}

	int getPriority(int i){
		return levels_arr[i].priority;
	}

	int getLevelCount(int i){
		return levels_arr[i].level_count;
	}

	int incLevelCount(int i){
		levels_arr[i].level_count+=1;
	}
};

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
