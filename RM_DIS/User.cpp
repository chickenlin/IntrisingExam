#include <iostream>
#include "Elevator.h"
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
using std::this_thread::sleep_for;

static const int NUM_OF_ELEVATOR = 2;

/* --- The structure use to pass the parameter to the child thread --- */
typedef struct {
    void *elevator;
    void *current;
    void *floor;
} ThreadArgs;



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

/*void *moniterThread(void *arg)
{
	// create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        cerr << "socket creation failed" << endl;
        return -1;
    }

    // 綁定 socket 到指定 IP 和 port
    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "bind failed" << endl;
        return -1;
    }

    // listen the request from client
    if (listen(server_fd, 1) < 0)
    {
        cerr << "listen failed" << endl;
        return -1;
    }

	cout << "server is waiting for client's integer..." << endl;

    // accept client's connection requirement
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0)
    {
        cerr << "accept failed" << endl;
        return -1;
    }

    cout << "client connected" << endl;
	
    pthread_detach(pthread_self());
    
    return 0;
}*/

int connect2Remote()
{
	cout << "create socket" << endl;
	// create socket
	int local_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (local_fd == -1)
	{
		cerr << "socket creation failed" << endl;
		return -1;
	}
	
	// connect to remote
	struct sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("140.115.52.74");
	server_addr.sin_port = htons(8080);
	
	if (connect(local_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		cerr << "connection failed" << endl;
		return -1;
	}

	cout << "connect to remote successfully" << endl;
	return local_fd;
}


int main()
{
	int local_fd = connect2Remote();
	
	static vector<Elevator> elevatorSet(NUM_OF_ELEVATOR);
    for (int i = 0; i < NUM_OF_ELEVATOR; i++)
    {
    	elevatorSet[i].set_id(i + 1);
    	elevatorSet[i].set_local_fd(local_fd);
    }
    int current, floor, minID, minDis;
	// pthread_t moniter;
	
	// pthread_create(&moniter, NULL, moniterThread, NULL);
	
    pthread_t tids[NUM_OF_ELEVATOR];
    string outBuf = "Hello server\n";
    cout << outBuf;
    if (send(local_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
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
            continue;
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
    if (send(local_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
    {
    	cerr << "Error: sending data failed" << endl;
	}
	
    close(local_fd);
    return 0;
}
