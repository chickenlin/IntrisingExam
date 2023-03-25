#include <iostream>
#include "Elevator.h"
#include <thread>
#include <chrono>
using namespace std;
using std::this_thread::sleep_for;

/* --- Use to check whether the floor is in correct range --- */
    bool Elevator::checkRange(int floor)
    {
        return (floor > 0 && floor < 11);
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
        cout << "Elevator" + to_string(id) + " is now on " + to_string(current_floor) + "th floor." << endl;
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

        /* --- The elevator move to `current` floor (The position of user) --- */
        cout << "Elevator" + to_string(id) + " is moving from " + to_string(current_floor) + " to " + to_string(current) + "th floor..." << endl;
        move(current);
        
        /* --- The elevator move to `floor` floor (The destination floor of user) --- */
        cout << "Elevator" + to_string(id) + " is moving from " + to_string(current) + " to " + to_string(floor) + "th floor..." << endl;
        move(floor);

        busy = false;
    }

