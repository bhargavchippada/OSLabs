#include <bits/stdc++.h>
#include <map>
#include <queue>
#include <string>

using namespace std;

void printVec(vector<int> vec){
	for(int i=0; i<vec.size(); i++) cout<<vec[i]<<" ";
}

struct request{
	int processID;
	int resourceID;
	int time;

	request(){};
	request(int pid, int rid, int t){
		processID = pid;
		resourceID = rid;
		time = t;
	}

	void print(){
		cout<<processID<<" "<<resourceID<<" "<<time<<endl;
	}
};

struct comp_RR{
	public:
 	int operator() ( const request& p1, const request &p2)
 	{
 		return p1.time>p2.time;
 	}
};

map<string,int> processPIDMap;
vector<string> processNames;
vector<string> resources;
map<string,int> resourceIDMap;

vector<int> resourceAllocationTable;
vector<queue<request> > resourceRequestQueue;
vector<vector<int> > resourceRequestedTable;
map<int, vector<int> > resourceNeeded;

vector<int> processStartTime;

priority_queue<request,vector<request>, comp_RR> RRqueue;

int current_time = 0;
int deadlock_maxtime = 0;
void parseProcesses(string line){
	int ind;
	string str;
	while((ind=line.find(','))!=-1){
		str = line.substr(0,ind);
		line = line.substr(ind+1);
		//cout<<str<<endl;
		processPIDMap[str] = processNames.size();
		//cout<<processPIDMap[str]<<endl;
		processNames.push_back(str);
	}
	str = line.substr(0,line.size()-1);
	//cout<<str<<endl;
	processPIDMap[str] = processNames.size();
	//cout<<processPIDMap[str]<<endl;
	processNames.push_back(str);
	//cout<<processPIDMap["P3"]<<endl;
}

void parseResources(string line){
	int ind;
	string str;
	while((ind=line.find(','))!=-1){
		str = line.substr(0,ind);
		line = line.substr(ind+1);
		//cout<<str<<endl;
		resourceIDMap[str] = resources.size();
		resources.push_back(str);
		
	}
	str = line.substr(0,line.size()-1);
	//cout<<str<<endl;
	resourceIDMap[str] = resources.size();
	resources.push_back(str);
	//cout<<resources[2]<<endl;
}

void parseRequests(string line){
	istringstream iss(line);
	string word;
	iss>>word;
	//cout<<word<<endl;
	iss>>line;
	//cout<<line<<endl;
	int i=0;
	int ind;
	string str;
	int val;
	string::size_type sz;
	vector<int> needed;
	while((ind=line.find(','))!=-1){
		str = line.substr(0,ind);
		val = stoi(str,&sz);
		line = line.substr(ind+1);
		//cout<<val<<" "<<i<<endl;
		if(val>=0){
			request *req = new request(processPIDMap[word],i,val);
			RRqueue.push(*req);
			needed.push_back(i);
		}
		i++;
	}
	str = line.substr(0,line.size());
	val = stoi(str,&sz);
	if(val>=0){
		request *req = new request(processPIDMap[word],i,val);
		RRqueue.push(*req);
		needed.push_back(i);
	}

	resourceNeeded[processPIDMap[word]] = needed;
	//cout<<val<<" "<<i<<endl;
}

void printRRS(){
	while(!RRqueue.empty()){
		request req = RRqueue.top();
		RRqueue.pop();
		req.print();
	}
}

void printRN(){
	cout<<"Resource Needed: "<<endl;
	for (map<int,vector<int> >::iterator it=resourceNeeded.begin(); it!=resourceNeeded.end(); ++it){
    	cout << it->first << " => ";
    	printVec(it->second);
    	cout<<endl;
	}
}

void printRA(){
	cout<<"Resource allocation table: "<<endl;
	for(int i=0; i<resourceAllocationTable.size();i++) cout<<resourceAllocationTable[i]<<" ";
	cout<<endl;
}

void initialize(){
	for(int i=0; i<resources.size(); i++){
		resourceAllocationTable.push_back(-1);
		resourceRequestQueue.push_back(*(new queue<request>()));
	}

	for(int i=0; i<processNames.size(); i++){
		processStartTime.push_back(-1);
		resourceRequestedTable.push_back(*(new vector<int>()));
		for(int j=0; j<resources.size(); j++){
			resourceRequestedTable[i].push_back(0);
		}
	}
}

bool checkDeadlock(){
	if(deadlock_maxtime>=4) {
		cout<<"Deadlock detected among ";
		for(int i=0; i<processStartTime.size(); i++){
			if(processStartTime[i]!=-2) cout<<processNames[i]<<" ";
		}
		cout<<endl;
		return true;
	}
	else return false;
}

bool checkDeadlockGraph(){

}

void checkRunning(){
	//printRA();
	for(int i=0; i<processNames.size(); i++){
		
		if(processStartTime[i]==-1){
			//check if this process has all it's resources
			bool has = true;
			vector<int> resourceNeed = resourceNeeded[i];
			for(int j=0; j<resourceNeed.size(); j++){
				if(resourceAllocationTable[resourceNeed[j]] != i){
					has = false;
					break;
				} else continue;
			}
			if(has){
				// process has all the resources needed to execute
				processStartTime[i] = current_time;
				cout<<processNames[i]<<" has started at "<<current_time<<endl;
				deadlock_maxtime=0;
			}
		}else if(processStartTime[i]==-2){
			//This process has finished in past
		}else{
			if(current_time==(processStartTime[i]+3)){
				//This process has finished
				processStartTime[i] = -2;
				vector<int> resourcesHeld = resourceNeeded[i];
				for(int j=0; j<resourcesHeld.size(); j++){
					resourceAllocationTable[resourcesHeld[j]] = -1; // resources are released
					cout<<resources[resourcesHeld[j]]<<" released by "<<processNames[i]<<" at "<<current_time<<endl;
					deadlock_maxtime=0;
				}
			}
		}
	}

	/*
	for(int i=0; i<processStartTime.size();i++){
		cout<<processStartTime[i]<<" ";
	}
	cout<<endl;
	*/
}

bool checkfinished(){
	bool finished = true;
	for(int i=0; i<processNames.size(); i++){
		if(processStartTime[i]!=-2) {
			finished=false;
			break;
		}
	}
	return finished;
}

void runSimulation(){
	
	while(!checkfinished()){
		//cout<<"Current time: "<<current_time<<endl;
		checkRunning();

		//This will check for pending requests and tries to allocate resources
		bool allocated = false;
		for(int i=0; i<resourceRequestQueue.size(); i++){
			if(resourceRequestQueue[i].size()==0) continue;
			else{
				request pendingReq = resourceRequestQueue[i].front();
				if(resourceAllocationTable[pendingReq.resourceID]==-1){
					cout<<resources[pendingReq.resourceID]<<" allocated to "<<processNames[pendingReq.processID]<<" at "<<current_time<<endl;
					deadlock_maxtime=0;
					resourceAllocationTable[pendingReq.resourceID] = pendingReq.processID;
					resourceRequestQueue[i].pop();
					resourceRequestedTable[pendingReq.processID][pendingReq.resourceID] = 0;
					allocated = true;
				}
			}
		}
		if(allocated) continue;
		
		//processing resource request
		if(RRqueue.size()!=0){
			request req = RRqueue.top();
			if(req.time==current_time){
				if(resourceAllocationTable[req.resourceID]==-1){
					//resource is free and can be allocated
					resourceAllocationTable[req.resourceID] = req.processID;
					cout<<resources[req.resourceID]+" allocated to "+processNames[req.processID]+" at "<<current_time<<endl;
					deadlock_maxtime=0;
					RRqueue.pop();
					continue;
				}else{
					//resource request added to pending queue
					cout<<processNames[req.processID]+" requests "<<resources[req.resourceID]<<" at "<<current_time<<endl;
					deadlock_maxtime=0;
					resourceRequestQueue[req.resourceID].push(req);
					resourceRequestedTable[req.processID][req.resourceID] = 1;
					RRqueue.pop();
				}
			}
		}

		if(checkDeadlock()) break;

		current_time++;
		deadlock_maxtime++;
	}
}


int main(int argc, char* argv[]){
	if(argc<2) {
		cerr<<"Input spec needed"<<endl;
		exit(0);
	}
	
	ifstream infile(argv[1]);
	if(infile.fail()) {
		cerr<<"Invalid file"<<endl;
		exit(0);
	}
	string line;
	getline(infile, line);
	parseProcesses(line);
	getline(infile, line);
	parseResources(line);
	
	while (getline(infile, line))
	{
	    parseRequests(line);
	}

	initialize();
	//printRRS();
	//printRN();
	//printRA();
	runSimulation();
}