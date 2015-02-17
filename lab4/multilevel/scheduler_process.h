#include <iostream>
#include <vector>
#include <queue>
#include "proc_structs.h"

using namespace std;

extern int current_time;
extern event_mgnr em;
extern vector<process> p_list;
extern int time_slice;
extern vector<process> p_list;
extern scheduler Scheduler;

struct pecb{
	int pid;
	int priority;
	int cpu_time;
	int io_time;
	int tslice;
	int level;
	int level_priority; // maintaing queue
	int end_time;
	int type; // 0 means IO event, 1 means admission event
	process *process_state;

	void print(){
		cout<<pid<<" "<<priority<<" "<<cpu_time<<" "<<io_time<<" "<<end_time<<" "<<type<<endl;
	}
};

class comp_ecb{
	public:
 	int operator() ( const pecb& p1, const pecb &p2)
 	{
 		return p1.end_time>p2.end_time;
 	}
};

class comp_pcb{
	public:
 	int operator() ( const pecb& p1, const pecb &p2)
 	{
 		if(p1.priority!=p2.priority){
 			return p1.priority<p2.priority;
 		}else{
 			return p1.level_priority > p2.level_priority;
 		}
 	}
};

class scheduler_process{
	pecb *running_process = NULL;

	public:
		priority_queue<pecb, vector<pecb>, comp_ecb> ecb_queue;
		priority_queue<pecb, vector<pecb>, comp_pcb> pcb_queue;

	void add_ecb(pecb ecb){
		ecb_queue.push(ecb);
	}

	void add_pcb(pecb pcb){
		pcb_queue.push(pcb);
	}

	void print_ecbqueue(){
		//removes the queue elements!!
		while(!ecb_queue.empty()){
			pecb ecb = ecb_queue.top();
			ecb_queue.pop();
			ecb.print();
		}
	}

	void print_pcbqueue(){
		//removes the queue elements!!
		while(!pcb_queue.empty()){
			pecb pcb = pcb_queue.top();
			pcb_queue.pop();
			pcb.print();
		}
	}

	void scheduler_method(){
		
		while(true){
			//cout<<"Hello  "<<current_time<<" "<<ecb_queue.size()<<" "<<pcb_queue.size()<<endl;
			//if(running_process!=NULL) cout<<"check "<<running_process->pid<<endl;
			pecb ecb_top; 
			pecb pcb_top;
			if(!ecb_queue.empty()){
				ecb_top = ecb_queue.top();
			}

			if(!pcb_queue.empty()){
				pcb_top = pcb_queue.top();
			}

			if(ecb_queue.empty() && pcb_queue.empty() && running_process==NULL) {
				break;
			}

			if(!ecb_queue.empty() && ecb_top.end_time == current_time){
				//cout<<"Inside"<<endl;
				//This is an ECB interrupt method
				if(running_process!=NULL){
					//the running process has to be ended if it's time slice is done
					if(running_process->cpu_time==0){
						//running process has completed its cpu burst
						running_process->end_time = current_time+running_process->io_time;
						running_process->type = 0;

						if(running_process->tslice == 0){
							//no need of demotion or promotion
						}else{
							//promotion is required
							int present_level = running_process->level;
							if(present_level<(Scheduler.no_of_levels-1)){
								running_process->level = present_level+1;
								running_process->priority = Scheduler.getPriority(present_level+1);
								em.add_event(current_time,9,running_process->pid,running_process->priority);
								//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Promoted to level " << present_level+1<<endl;
							}
						}
						em.add_event(current_time,6,running_process->pid,-1);
						em.add_event(current_time,3,running_process->pid,-1);
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(6)<<endl;
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(3)<<endl;
						ecb_queue.push(*running_process);
						running_process = NULL;
						continue;
					}else if(running_process->tslice==0){
						//pre-empted
						running_process->end_time = -1;
						running_process->type = 0;
						//demotion
						int present_level = running_process->level;
						if(present_level>0){
							running_process->level = present_level-1;
							running_process->level_priority = Scheduler.getLevelCount(present_level-1);
							Scheduler.incLevelCount(present_level-1);
							running_process->priority = Scheduler.getPriority(present_level-1);
							running_process->tslice = Scheduler.getTimeSlice(present_level-1);
							em.add_event(current_time,10,running_process->pid,Scheduler.getTimeSlice(present_level-1));
							//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Demoted to level" << present_level-1<<endl;
						}else{
							running_process->level = present_level;
							running_process->level_priority = Scheduler.getLevelCount(present_level);
							Scheduler.incLevelCount(present_level);
							running_process->priority = Scheduler.getPriority(present_level);
							running_process->tslice = Scheduler.getTimeSlice(present_level);
						}
						em.add_event(current_time,2,running_process->pid,-1);
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"End of time slice"<<endl;
						pcb_queue.push(*running_process);
						running_process = NULL;
						continue;
					}else if(ecb_top.priority > running_process->priority){
						//pre-empted
						int present_level = running_process->level;
						running_process->level_priority = Scheduler.getLevelCount(present_level);
						Scheduler.incLevelCount(present_level);
						running_process->tslice = Scheduler.getTimeSlice(present_level);
						em.add_event(current_time,7,running_process->pid,-1);
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Pre-empted"<<endl;
						pcb_queue.push(*running_process);
						running_process = NULL;
						continue;
					}else{
						//don't pre-empt
					}
					
				}

				if(ecb_top.type==1){
					// This is an admission event and we needn't do anything since it's event is already added by default to event manager
					//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(1)<<endl;
					process *proc = ecb_top.process_state;
					pair<int,int> work = proc->getWork();
					//cout<<"Work :: "<<work.first<<" "<<work.second<<endl;	
					if(work.first==-1 && work.second==-1){
						// do nothing, this process is complete
						em.add_event(current_time,8,ecb_top.pid,-1);
						//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Terminated"<<endl;
						ecb_queue.pop();
						continue;
					}else{
						//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Add for scheduling"<<endl;
						ecb_queue.pop();
						ecb_top.cpu_time = work.first;
						ecb_top.io_time = work.second;
						ecb_top.end_time = -1;
						ecb_top.type = -1;
						int present_level = ecb_top.level;
						ecb_top.tslice = Scheduler.getTimeSlice(present_level);
						ecb_top.level_priority = Scheduler.getLevelCount(present_level);
						Scheduler.incLevelCount(present_level);
						pcb_queue.push(ecb_top);
					}
					
				}else{
					process *proc = ecb_top.process_state;
					pair<int,int> work = proc->getWork();
					//cout<<"Work :: "<<work.first<<" "<<work.second<<endl;
					em.add_event(current_time,4,ecb_top.pid,-1);
					//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(4)<<endl;	
					if(work.first==-1 && work.second==-1){
						// do nothing, this process is complete
						em.add_event(current_time,8,ecb_top.pid,-1);
						//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Terminated"<<endl;
						ecb_queue.pop();
						continue;
					}else{
						//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Add for scheduling"<<endl;
						ecb_queue.pop();
						ecb_top.cpu_time = work.first;
						ecb_top.io_time = work.second;
						ecb_top.end_time = -1;
						ecb_top.type = -1;
						int present_level = ecb_top.level;
						ecb_top.tslice = Scheduler.getTimeSlice(present_level);
						ecb_top.level_priority = Scheduler.getLevelCount(present_level);
						Scheduler.incLevelCount(present_level);
						pcb_queue.push(ecb_top);
					}
				}

				continue;
			}


			//cout<<"Running Process "<<running_process->cpu_time<<endl;
			if(running_process!=NULL){
				
				//cout<<"Running Process "<<running_process->cpu_time<<endl;
				if(running_process->cpu_time==0){
					//running process has completed its cpu burst
					running_process->end_time = current_time+running_process->io_time;
					running_process->type = 0;
					if(running_process->tslice == 0){
							//no need of demotion or promotion
					}else{
						//promotion is required
						int present_level = running_process->level;
						if(present_level<(Scheduler.no_of_levels-1)){
							running_process->level = present_level+1;
							running_process->priority = Scheduler.getPriority(present_level+1);
							em.add_event(current_time,9,ecb_top.pid,running_process->priority);
							//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Promoted to level "<<present_level+1<<endl;
						}
					}
					em.add_event(current_time,6,running_process->pid,-1);
					em.add_event(current_time,3,running_process->pid,-1);
					//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(6)<<endl;
					//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(3)<<endl;
					ecb_queue.push(*running_process);
					running_process = NULL;
					continue;
				}else if(running_process->tslice==0){
					//pre-empted
					running_process->end_time = -1;
					running_process->type = 0;
					//demotion
					int present_level = running_process->level;
					if(present_level>0){
						running_process->level = present_level-1;
						running_process->level_priority = Scheduler.getLevelCount(present_level-1);
						Scheduler.incLevelCount(present_level-1);
						running_process->priority = Scheduler.getPriority(present_level-1);
						running_process->tslice = Scheduler.getTimeSlice(present_level-1);
						em.add_event(current_time,10,ecb_top.pid,running_process->priority);
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Demoted to level "<<present_level-1<<endl;
					}else{
						running_process->level = present_level;
						running_process->level_priority = Scheduler.getLevelCount(present_level);
						Scheduler.incLevelCount(present_level);
						running_process->priority = Scheduler.getPriority(present_level);
						running_process->tslice = Scheduler.getTimeSlice(present_level);
					}

					em.add_event(current_time,2,running_process->pid,-1);
					//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"End of time slice"<<endl;
					pcb_queue.push(*running_process);
					running_process = NULL;
					continue;
				}else{
					running_process->cpu_time=running_process->cpu_time-1;
					running_process->tslice=running_process->tslice-1;
				}
			}else if(!pcb_queue.empty()){
				//start new process from pcb_list

				pcb_queue.pop();
				
				running_process = new pecb();
				running_process->pid = pcb_top.pid;
				running_process->priority = pcb_top.priority;
				running_process->cpu_time = pcb_top.cpu_time;
				running_process->io_time = pcb_top.io_time;
				running_process->tslice = pcb_top.tslice;
				running_process->end_time = pcb_top.end_time;
				running_process->level = pcb_top.level;
				running_process->level_priority = pcb_top.level_priority;
				running_process->type = pcb_top.type; // 0 means IO event, 1 means admission event
				running_process->process_state = pcb_top.process_state;
				
				//cout<<"bye "<<running_process->cpu_time<<endl;
				em.add_event(current_time,5,pcb_top.pid,-1);
				//cout<<"PID :: "<<pcb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(5)<<endl;
				continue;	
			}

			current_time++;
		}

		cout<<"Scheduling Completed!\n";
		
	}

};

