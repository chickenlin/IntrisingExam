#include <iostream>
#include "Elevator.h"
#include <thread>
#include <chrono>
using namespace std;
using std::this_thread::sleep_for;

static const int NUM_OF_ELEVATOR = 2;

/* --- The structure use to pass the parameter to the child thread --- */
typedef struct {
    void *elevator;
    void *current;
    void *floor;
} ThreadArgs;



void* elevatorThread(void *arg)
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

int main()
{
    static vector<Elevator> elevatorSet(NUM_OF_ELEVATOR);
    for (int i = 0; i < NUM_OF_ELEVATOR; i++)
    	elevatorSet[i].set_id(i + 1);
    int current, floor, minID, minDis;
    pthread_t tids[NUM_OF_ELEVATOR];

    while (1)
    {
        /* --- Pause the program until user input --- */
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
        pthread_create(&tids[minID], NULL, elevatorThread, (void*)&args);
    }
    return 0;
}
