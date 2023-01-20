# CAB403 - Systems Programming 
## Car Park Management System Simulator Assignment

![QUT](https://img.shields.io/badge/QUT-Tyrone%20Nolasco-darkblue?style=plastic) ![Year](https://img.shields.io/badge/Year-2019-red?style=plastic) ![Semester](https://img.shields.io/badge/Semester-2-red?style=plastic) ![Language](https://img.shields.io/badge/C-grey?style=plastic&logo=C)

CAB403 is a computer science unit that expands upon concepts taught in CAB202 and delves deeper into C's intricacies regarding memory management, threading and file protection. This repository is for **Assignment 2** of the subject wherein we had to develop a **Car Park Management System Simulator (CPMSS)** as per the univerity's spec.

### Description
#### Purpose
- The purpose of the assignment was to simulate the behaviour of a car park written in C to match with the specification provided in the assignment sheet provided by the university. The system tracked the state of cars, boom gates, billing and accounted for abnormal scenarios as an exploration of memory management, threading and safety critical programming
- The assignment requires `gcc` to be installed on the machine in order to run `make` commands
- This code is for display purposes only and should not be copied

#### Environment
- WSL2 on Windows 10 was the environment used to build and run the project as the `gcc` build commands worked best there, running as an alternative to the VMBox provided by the university.  
- `make clean && make && make run` is the command to clean the project and run it in the terminal window
- `make run dev` is the command to run the project with [`valgrind`](https://valgrind.org/docs/manual/quick-start.html#quick-start.intro), useful for checking memory leaks during the debugging process.
