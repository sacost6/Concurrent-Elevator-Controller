﻿# Concurrent-Elevator-Controller

## Project Background
A M story highrise has N elevators in a single “elevator bank”, each able to serve every floor of the building. Contrary from most elevator designs, there are no buttons to choose a destination floor inside the elevator. Instead, the elevator lobby at each floor has M individual “destination floor” buttons, and passengers choose their destination by pushing the appropriate button. Once a button is pushed, a display near the buttons tells the passenger which elevator door to wait by. In this assignment, elevators serve only one passenger at a time.

## Project Summary
In this project the goal is to create an elevator controller that ensures that all passengers receive service, minimizing the amount of time it takes to serve all of the passengers, and how much CPU time is used to run the controller. The template code works correctly, but is terribly slow. In part, this is because passengers often “miss” the elevator (don’t get in in time), or miss their floor (stay on the elevator). In part, it is because the elevator stops at every floor, and we only use one elevator.

## Requirements:
* Achieve multiple elevators using threads.
* Minimize CPU time of program.
* Topics Learned Upon Completion
* Event-based concurrency in C
* Using pthread_barriers and pthread_mutex locks for performance enhancement
* Using pthread_cond condition variables for performance enhancment
* Real-world applications in embedded systems
