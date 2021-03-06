#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "event_manager.h"
#include "scheduler_process.h"

using namespace std;

event_mgnr em;

struct scheduling_level {
	int level_no;
	int priority;
	int time_slice;	//either some integer or N
};

struct scheduler {
	int no_of_levels;
	vector<scheduling_level> levels_arr;
};

scheduler_process sched_proc;

int current_time = 0;

scheduler Scheduler;
vector<process> p_list;

void handling_PROCESS_SPEC_file(){
	string line, line2;
	int pid, prior;
	int adm;
	int iter;
	int cpu_t, io_t;
	ifstream infile("PROCESS_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="PROCESS"){
			process *proc = new process();
			getline(infile, line2);
			std::istringstream iss(line2);
		        if (!(iss >> pid >> prior >> adm)) { cout<<"Error handling_PROCESS_SPEC_file\n"; break; } // error
		    
			proc->pid = pid;
			proc->start_priority = prior;
			proc->admission = adm;

			pecb temp_pecb;
			temp_pecb.pid = pid;
			temp_pecb.priority = prior;
			temp_pecb.cpu_time = 0;
			temp_pecb.io_time = adm;
			temp_pecb.end_time = adm;
			temp_pecb.type = 1;

			getline(infile, line2);
			while(line2 != "END"){
				std::istringstream iss(line2);
				process_phase pp;
			        if (!(iss >> iter >> cpu_t >> io_t)) { cout<<"Error handling_PROCESS_SPEC_file\n"; break; } // error
			    
				pp.itrs = iter;
		    	pp.cpu_b = cpu_t;
		    	pp.io_b = io_t;
		    	(proc->phases).push_back(pp);
		    	getline(infile, line2);
			}
			p_list.push_back(*proc);
			temp_pecb.process_state = proc;
			sched_proc.add_ecb(temp_pecb);
			
			em.add_event(proc->admission,1,proc->pid);	//event type "1" represents "process admission event"

		}
	}
}

int string_to_integer(string str)
{
	int r=1,s=0,l=str.length(),i;
	for(i=l-1;i>=0;i--)
	{
		s = s + ((str[i] - '0')*r);
		r *= 10;
	}
	return s;
}

void handling_SCHEDULER_SPEC_file(){
	string line, line2;
	int level_count;
	int prior;
	int s_lvl;
	int t_slice;
	string t_slice1;
	ifstream infile("SCHEDULER_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="SCHEDULER"){
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> level_count)) { break; } // error
			
			Scheduler.no_of_levels = level_count;
			for(int i=0; i<level_count; i++){
				getline(infile, line2);
				std::istringstream iss(line2);
				if (!(iss >> s_lvl >> prior >> t_slice1)) { break; } // error
				scheduling_level scl;
				if(t_slice1 == "N")
					t_slice = 9999;
				else
					t_slice = string_to_integer(t_slice1);
				scl.level_no = s_lvl;
				scl.priority = prior;
				scl.time_slice = t_slice;
				
				Scheduler.levels_arr.push_back(scl);
			}
		}
	}
}

int main()
{
	handling_PROCESS_SPEC_file();
	handling_SCHEDULER_SPEC_file();
	//sched_proc.print_ecbqueue();
	sched_proc.scheduler_method();
	//processing events
	
	event next;
	while(!em.is_empty())
	{
		next = em.next_event();
		next.printEvent();
	}
	
	return 0;
}
