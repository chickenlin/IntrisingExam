# IntrisingExam
A program simulate a system with two elevator. Once you want to take a elevator, enter `Start` into the terminal, then input `the floor you are located now` and `another floor you want to go`. The program will distribute an available elevator for you. If there is no available elevator, please wait for a moment. Once you want to exit, enter `Exit` to the terminal.
## Development environment
  **OS**: Ubuntu 22.04 LTS
  
  **Language**: GCC version 8.1.0 (C++17)

## Program overview
**Object**

  `Elevator`: the object present the elevator, I assume the elevator is at first floor initailly

**Attribute**

  `current_floor (int)`: the elevator's position
  `busy (bool)`: Indicate the elevator is busy or not

**Method**

  `checkRange`: Use to check whether the input floor is in correct range
  `display_floor`: display the position of the elevator
  `isBusy`: return the elevator is busy or not
  `move (int floor)`: a method which tell the elevator to move to floor
  `callElevator (int current, int floor)`: the api for user to take the elevator from current to floor

**Other function**

  `elevatorThread`: the entry of the child thread, which will call `callElevator` subsequently

***Warning***: Due to pthread, please execute this program on linux platform
