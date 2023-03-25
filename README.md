# IntrisingExam
A program simulate a system with two elevator. Once you want to take a elevator, enter `Start` into the terminal, then input `the floor you are located now` and `another floor you want to go`. The program will distribute an available elevator for you. If there is no available elevator, please wait for a moment. Once you want to exit, enter `Exit` to the terminal.

## Update
  **[2023/03/23]** (*Update*) Add elevator's id and method to choose the nearest elevator
  
  **[2023/03/23]** (*Update*) Add unittest's file and seperate the interface and implementation

## Development environment
  **OS**: Ubuntu 22.04 LTS
  
  **Language**: GCC version 8.1.0 (C++17)

## Program overview

**Command to compile**
```shell=
  $ g++ User.cpp Elevator.cpp -o User
```

**Elevator's Object**

  `Elevator`: the object present the elevator, I assume the elevator is at first floor initailly

**Attribute**

  `id`: the elevator's position

  `current_floor (int)`: the elevator's position
  
  `busy (bool)`: Indicate the elevator is busy or not

**Method**

  `checkRange`: Use to check whether the input floor is in correct range
  
  `display_floor`: display the position of the elevator
  
  `isBusy`: return the elevator is busy or not
  
  `move (int floor)`: a method which tell the elevator to move to `floor`
  
  `callElevator (int current, int floor)`: the api for user to take the elevator from `current` to `floor`

**Other function**

  `elevatorThread`: the entry of the child thread, which will call `callElevator` subsequently
  
## Unit test
I use GoogleTest to conduct unit test, which use Google's C++ test framework.

**Installment**
```shell=
  $ git clone https://github.com/google/googletest
  $ cd googletest
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make -j$(nproc)
  $ sudo make install
```

**Command to compile**
```shell=
  $ g++ unittest.cpp Elevator.cpp -o unittest -std=c++17 -lgtest -lpthread
```

***Warning***: Due to pthread, please execute these programs on linux platform
