#include <iostream>
#include "Elevator.h"
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define FROM_USER		0
#define FROM_MONITER	1

using namespace std;
using std::this_thread::sleep_for;

static const int NUM_OF_ELEVATOR = 2;

/* --- The structure use to pass the parameter to the child thread --- */
typedef struct {
    void *elevator;
    void *current;
    void *floor;
} ThreadArgs;

vector<Elevator> elevatorSet;
pthread_t tids[NUM_OF_ELEVATOR];

void* localThread(void *arg)
{
    ThreadArgs *args = (ThreadArgs*)arg;

    void *elevator = args->elevator;
    void *current = args->current;
    void *floor = args->floor;

    Elevator *eletoCall = (Elevator*)elevator;
    eletoCall->callElevator(*(int*)current, *(int*)floor);
    
    pthread_detach(pthread_self());
    
    return 0;
}
void *moniterThread(void *arg)
{
	// create socket
    int elevator_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (elevator_fd == -1)
    {
        cerr << "socket creation failed" << endl;
		pthread_detach(pthread_self());
		return nullptr;
    }

    // bind socket to specific IP and port
    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(elevator_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "bind failed" << endl;
		pthread_detach(pthread_self());
		return nullptr;
    }

    // listen the request from client
    if (listen(elevator_fd, 1) < 0)
    {
        cerr << "listen failed" << endl;
		pthread_detach(pthread_self());
		return nullptr;
    }

    // accept client's connection requirement
    int moniter_fd = accept(elevator_fd, nullptr, nullptr);
    if (moniter_fd < 0)
    {
        cerr << "accept failed" << endl;
		pthread_detach(pthread_self());
		return nullptr;
    }

    cout << "moniter connected" << endl;
	
	while (1)
	{
		// receive `current`, `floor` send from moniter
		int current, floor;
		if (recv(moniter_fd, &current, sizeof(current), 0) < 0)
		{
			pthread_detach(pthread_self());
			return nullptr;
		}
		if (recv(moniter_fd, &floor, sizeof(floor), 0) < 0)
		{
			cerr << "receive failed" << endl;
			pthread_detach(pthread_self());
			return nullptr;
		}
		
		if (current == 0 && floor == 0) break;
		bool avail = true;
		int minDis, minID;
		
		for (int i = 0; i < NUM_OF_ELEVATOR; i++)
		{
			avail = avail && elevatorSet[i].isbusy();
			if (elevatorSet[i].isbusy() == false)
			{
			  	avail = false;
			}
		}

		// call the elevator
		if (avail)
		{
			cout << "No elevators is useable, please wait for a moment!" << endl;
			string outBuf = "No elevators is useable, please wait for a moment!";
			if (send(elevatorSet[0].get_fd(), outBuf.c_str(), outBuf.length(), 0) < 0)
			{
				cerr << "Error: sending data failed" << endl;
			}
		}

		minDis = INT_MAX;
		
		for (int i = 0; i < NUM_OF_ELEVATOR; i++)
		{
			if (elevatorSet[i].isbusy() == false)
		 		{
				if (abs(elevatorSet[i].get_floor() - current) < minDis)
				{
				    minID = i;
				    minDis = abs(elevatorSet[i].get_floor() - current);
				}
			}
		}
		
		elevatorSet[minID].callElevator(current, floor);
	}
	
    pthread_detach(pthread_self());
    return nullptr;
}

int connect2Remote()
{
	cout << "create socket" << endl;
	// create socket
	int remote_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (remote_fd == -1)
	{
		cerr << "socket creation failed" << endl;
		return -1;
	}
	
	// connect to remote
	struct sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("140.115.52.74");
	server_addr.sin_port = htons(8080);
	
	if (connect(remote_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		cerr << "connection failed" << endl;
		return -1;
	}

	cout << "connect to remote successfully" << endl;
	return remote_fd;
}

int main()
{
	static pthread_t moniter;
	pthread_create(&moniter, NULL, moniterThread, NULL);
	
	int remote_fd = connect2Remote();
	
	elevatorSet.resize(NUM_OF_ELEVATOR);
    for (int i = 0; i < NUM_OF_ELEVATOR; i++)
    {
    	elevatorSet[i].set_id(i + 1);
    	elevatorSet[i].set_fd(remote_fd);
    }
    int current, floor, minID, minDis;
	
    string outBuf = "Hello server";
    if (send(remote_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
    {
    	cerr << "Error: sending data failed" << endl;
	}
    while (1)
    {
        // Pause the program until user input
        cout << "Press Start to call the elevator, Exit to end the program" << endl;
        string in;
        cin >> in;
        if (in.compare("Exit") == 0) break;

        bool avail = true;
		
		for (int i = 0; i < NUM_OF_ELEVATOR; i++)
		{
		    avail = avail && elevatorSet[i].isbusy();
		    if (elevatorSet[i].isbusy() == false)
		    {
		    	avail = false;
		    }
		}

		if (avail)
		{
		    cout << "No elevators is useable, please wait for a moment!" << endl;
		    string outBuf = "No elevators is useable, please wait for a moment!";
			if (send(elevatorSet[0].get_fd(), outBuf.c_str(), outBuf.length(), 0) < 0)
			{
				cerr << "Error: sending data failed" << endl;
			}
		}
		
		cout << "Input your current floor and your destination floor:" << endl;
		cin >> current >> floor;
		minDis = INT_MAX;
	
		for (int i = 0; i < NUM_OF_ELEVATOR; i++)
		{
		    if (elevatorSet[i].isbusy() == false)
		    {
		        if (abs(elevatorSet[i].get_floor() - current) < minDis)
		        {
		            minID = i;
		          	minDis = abs(elevatorSet[i].get_floor() - current);
		        }
		    }
		}
		
		ThreadArgs args = {&elevatorSet[minID], &current, &floor};
		pthread_create(&tids[minID], NULL, localThread, (void*)&args);
    }
    outBuf = "EndOfRemote";
    cout << outBuf;
    if (send(remote_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
    {
    	cerr << "Error: sending data failed" << endl;
	}
	
    close(remote_fd);
    return 0;
}
