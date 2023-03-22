#include <bits/stdc++.h>
#include <thread>
#include <chrono>
using namespace std;
using std::this_thread::sleep_for;

/* --- The structure use to pass the parameter to the child thread --- */
typedef struct {
    void *elevator;
    void *current;
    void *floor;
} ThreadArgs;

class Elevator
{
    int current_floor;

    /* --- Indicate the elevator is busy or not --- */
    bool busy;

    /* --- Use to check whether the floor is in correct range --- */
    bool checkRange(int floor)
    {
        return (floor > 0 && floor < 11);
    }
public:
    /* --- Assume the elevator is at first floor initailly --- */
    Elevator(): current_floor(1), busy(false){}

    int get_floor()
    {
        return current_floor;
    }

    void display_floor()
    {
        cout << "The elevator is now on " + to_string(current_floor) + "th floor." << endl;
    }

    bool isbusy()
    {
        return busy;
    }

    void move(int floor)
    {
        /* --- The elevator move to `current` floor (The user is here) --- */
        //current_floor = current;
        //display_floor();

        int direct;
        if (floor > current_floor)  direct = 1;
        else                        direct = -1;
        while (current_floor != floor)
        {
            current_floor += direct;
            display_floor();
            sleep_for(std::chrono::seconds(1));
        }
    }

    void callElevator(int current, int floor)
    {
        if (!checkRange(current) || !checkRange(floor))
        {
            cout << "Please input valid floor number!\n" << endl;
            return;
        }

        busy = true;

        /* --- The elevator move to `current` floor (The position of user) --- */
        cout << "Moving from " + to_string(current_floor) + " to " + to_string(current) + "th floor..." << endl;
        move(current);
        
        /* --- The elevator move to `floor` floor (The destination floor of user) --- */
        cout << "Moving from " + to_string(current) + " to " + to_string(floor) + "th floor..." << endl;
        move(floor);

        busy = false;
    }
};

void* elevatorThread(void *arg)
{
    ThreadArgs *args = (ThreadArgs*)arg;

    void *elevator = args->elevator;
    void *current = args->current;
    void *floor = args->floor;

    Elevator *eletoCall = (Elevator*)elevator;
    eletoCall->callElevator(*(int*)current, *(int*)floor);
    return 0;
}

int main()
{
    static vector<Elevator> elevatorSet(2);
    int current, floor;
    pthread_t tids[2];

    while (1)
    {
        /* --- Pause the program until user input --- */
        cout << "Press Start to call the elevator, Exit to end the program" << endl;
        string in;
        cin >> in;
        if (in.compare("Exit") == 0) break;

        bool avail = true;
        for (int i = 0; i < elevatorSet.size(); i++)
        {
            avail = avail && elevatorSet[i].isbusy();
            if (elevatorSet[i].isbusy() == false)
            {
                cout << "Input your current floor and your destination floor:" << endl;
                cin >> current >> floor;
                cout << current << floor;
                ThreadArgs args = {&elevatorSet[i], &current, &floor};
                pthread_create(&tids[i], NULL, elevatorThread, (void*)&args);
                break;
            }
        }

        if (avail)
        {
            cout << "No elevators is useable, please wait for a moment!" << endl;
            continue;
        }
    }
    return 0;
}