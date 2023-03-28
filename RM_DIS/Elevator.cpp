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

/* --- Use to check whether the floor is in correct range --- */
    bool Elevator::checkRange(int floor)
    {
        return (floor > 0 && floor < 11);
    }
	
	void Elevator::set_remote_fd(int fd)
	{
		remote_fd = fd;
	}
	
	void Elevator::set_local_fd(int fd)
	{
		local_fd = fd;
	}
    void Elevator::set_id(int ID)
    {
    	id = ID;
    }

    int Elevator::get_floor()
    {
        return current_floor;
    }

    void Elevator::display_floor()
    {
    	string outBuf = "Elevator" + to_string(id) + " is now on " + to_string(current_floor) + "th floor.\n";
        cout << outBuf;
        if (send(local_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
		{
			cerr << "Error: sending data failed" << endl;
		}
    }

    bool Elevator::isbusy()
    {
        return busy;
    }

    void Elevator::move(int floor)
    {
        int direct;
        if (floor > current_floor)  direct = 1;
        else                        direct = -1;
        while (current_floor != floor)
        {
            sleep_for(std::chrono::seconds(1));
            current_floor += direct;
            display_floor();
        }
    }

    void Elevator::callElevator(int current, int floor)
    {
        if (!checkRange(current) || !checkRange(floor))
        {
            cout << "Please input valid floor number!\n" << endl;
            return;
        }

        busy = true;

        // The elevator move to `current` floor (The position of user)
        string outBuf = "Elevator" + to_string(id) + " is moving from " + to_string(current_floor) + " to " + to_string(current) + "th floor...\n";
        cout << outBuf;
        if (send(local_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
		{
			cerr << "Error: sending data failed" << endl;
		}
        move(current);
        
        /* --- The elevator move to `floor` floor (The destination floor of user) --- */
        outBuf = "Elevator" + to_string(id) + " is moving from " + to_string(current) + " to " + to_string(floor) + "th floor...\n";
        cout << outBuf;
        if (send(local_fd, outBuf.c_str(), outBuf.length(), 0) < 0)
		{
			cerr << "Error: sending data failed" << endl;
		}
        move(floor);

        busy = false;
    }

