#include <iostream>
#include <vector>
#include <queue>
#include "proc_structs.h"
using namespace std;

extern int current_time;
extern event_mgnr em;
extern vector<process> p_list;

struct pecb{
	int pid;
	int priority;
	int cpu_time;
	int io_time;
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
 		return p1.priority<p2.priority;
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
			//if(running_process!=NULL) cout<<"check "<<running_process->cpu_time<<endl;
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
					//the running process has to be pre-empted
					if(running_process->cpu_time==0){
						//running process has completed its cpu burst
						running_process->end_time = current_time+running_process->io_time;
						running_process->type = 0;
						em.add_event(current_time,6,running_process->pid);
						em.add_event(current_time,3,running_process->pid);
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(6)<<endl;
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(3)<<endl;
						ecb_queue.push(*running_process);
						running_process = NULL;
						continue;
					}else{
						em.add_event(current_time,7,running_process->pid);
						//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<"Process Pre-Empted"<<endl;	
						pcb_queue.push(*running_process);
						running_process = NULL;
						continue;
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
						pcb_queue.push(ecb_top);
					}
					
				}else{
					process *proc = ecb_top.process_state;
					pair<int,int> work = proc->getWork();
					//cout<<"Work :: "<<work.first<<" "<<work.second<<endl;
					em.add_event(current_time,4,ecb_top.pid);
					//cout<<"PID :: "<<ecb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(4)<<endl;	
					if(work.first==-1 && work.second==-1){
						// do nothing, this process is complete
						em.add_event(current_time,9,ecb_top.pid);
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
					em.add_event(current_time,6,running_process->pid);
					em.add_event(current_time,3,running_process->pid);
					//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(6)<<endl;
					//cout<<"PID :: "<<running_process->pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(3)<<endl;
					ecb_queue.push(*running_process);
					running_process = NULL;
					continue;
				}else{
					running_process->cpu_time=running_process->cpu_time-1;
				}
			}else if(!pcb_queue.empty()){
				//start new process from pcb_list
				em.add_event(current_time,8,pcb_top.pid);
				pcb_queue.pop();
				
				running_process = new pecb();
				running_process->pid = pcb_top.pid;
				running_process->priority = pcb_top.priority;
				running_process->cpu_time = pcb_top.cpu_time;
				running_process->io_time = pcb_top.io_time;
				running_process->end_time = pcb_top.end_time;
				running_process->type = pcb_top.type; // 0 means IO event, 1 means admission event
				running_process->process_state = pcb_top.process_state;
				
				//cout<<"bye "<<running_process->cpu_time<<endl;
				
				//cout<<"PID :: "<<pcb_top.pid<<" TIME :: "<<current_time<<" EVENT :: "<<getTypeOfEvent(5)<<endl;
				continue;	
			}

			current_time++;
		}

		//cout<<"Scheduling Completed!\n";
	}


};

