#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

string getTypeOfEvent(int type)
{
		switch(type)
		{
			case 1:
				return "Process Admitted";
				break;
			case 2:
				return "Time slice ended";
				break;
			case 3:
				return "IO started";
				break;
			case 4:
				return "IO burst completed";
				break;
			case 5:
				return "CPU started";
				break;
			case 6:
				return "CPU burst completed";
				break;
			case 7:
				return "Process pre-empted";
				break;
			case 8:
				return "Process terminated";
				break;
			default:
				return "";
				break;
		}
}

struct event{
	int end_t;	//event occurrence time 
	int type;	//event type
	int pid;	//process id
	int level;

	void printEvent(){
		cout<<"PID :: "<<pid<<" TIME :: "<<end_t<<" EVENT :: "<<getTypeOfEvent(type);
		if(type==9) cout<<"Process promoted to "<<level;
		else if(type==10) cout<<"Process demoted to "<<level;
		cout<<endl;
	}
};

class comp{
public:
 	int operator() ( const event& p1, const event &p2)
 	{
 		return p1.end_t>p2.end_t;
 	}
};

class event_mgnr {
	public:
		priority_queue<event, vector<event>, comp> event_table;

	//function for adding an event in the event table
	void add_event(int end_t, int type, int pid, int level)
	{
		event ev;
		ev.end_t = end_t;
		ev.type = type;
		ev.pid = pid;
		ev.level = level;
		event_table.push(ev);
	}

	//Is event table empty..?
	bool is_empty()
	{
		return event_table.empty();
	}
	
	//function for returning the top_most entry of the event table
	event next_event()
	{
		event ev = event_table.top();
		event_table.pop();
		return ev;
	}

};
