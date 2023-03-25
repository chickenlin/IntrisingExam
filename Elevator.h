#include <bits/stdc++.h>
#include <thread>
#include <chrono>
using namespace std;
using std::this_thread::sleep_for;

class Elevator
{
    int current_floor;

    int id;

    /* --- Indicate the elevator is busy or not --- */
    bool busy;

    /* --- Use to check whether the floor is in correct range --- */
    bool checkRange(int floor);
    
public:
    /* --- Assume the elevator is at first floor initailly --- */
    Elevator(): current_floor(1), busy(false){}

    void set_id(int ID);

    int get_floor();

    void display_floor();

    bool isbusy();

    void move(int floor);

    void callElevator(int current, int floor);
};

