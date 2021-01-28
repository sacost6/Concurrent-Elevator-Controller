#include "hw6.h"
#include <stdio.h>
#include<pthread.h>

struct Elevator {
	int barrier1, barrier2;
	pthread_mutex_t lock; 
	pthread_barrier_t elevatorBlock1, elevatorBlock2, barrier;	
	int current_floor;
	int direction;
	int occupancy;
	int target;
	int id;
	int to,from;
	int currPassenger;
	enum {ELEVATOR_ARRIVED=1, ELEVATOR_OPEN=2, ELEVATOR_CLOSED=3} state;	
}elevators[ELEVATORS];

void scheduler_init() {

    for(int i = 0; i < ELEVATORS; i++) {
	struct Elevator *e = &elevators[i];
	
    	pthread_mutex_init((void*)&(e->lock), NULL);
	e->current_floor=0;
    	e->direction=-1;
    	e->occupancy=0;
	e->target = -1;
	e->id = i;
    	e->state=ELEVATOR_CLOSED;
 	pthread_barrier_init((void *)&(e->elevatorBlock1), NULL, 2);
        pthread_barrier_init((void *)&(e->elevatorBlock2), NULL , 2);
	pthread_barrier_init((void*)&(e->barrier), NULL, 2);
    }
}


void passenger_request(int passenger, int from_floor, int to_floor, 
        void (*enter)(int, int), void(*exit)(int, int))
{	
    int assignedElevator = -1; 
    struct Elevator *e = &elevators[0];
    int available = 0; 
    while(available == 0) {

	for(int i = 0; i < ELEVATORS; i++) {
	    e = &elevators[i]; 
	    pthread_mutex_lock((void*)&(e->lock));
	    if(e->target == -1){ 
    		e->target = from_floor;
		e->to = to_floor;
		e->from = from_floor;
         	available = 1; 
	    	pthread_mutex_unlock((void*)&(e->lock));
		break; 
	    }

	    pthread_mutex_unlock((void*)&(e->lock));
	}
    }
    // wait for the elevator to arrive at our origin floor, then get in
    int waiting = 1;
    while(waiting) {
	pthread_barrier_wait((void*)&(e->elevatorBlock2));
        pthread_mutex_lock((void*)&(e->lock));
	if(e->current_floor == from_floor && e->state == ELEVATOR_OPEN && e->occupancy==0) {
	    enter(passenger, e->id);
            e->occupancy++;
    	    e->target = to_floor; 
	    waiting=0;
        }
	pthread_mutex_unlock((void*)&(e->lock));
    }
    pthread_barrier_wait((void*)&(e->elevatorBlock1));
    // wait for the elevator at our destination floor, then get out
    int riding=1;
    while(riding) {
	pthread_mutex_lock((void*)&(e->lock));
        if(e->current_floor == to_floor && e->state == ELEVATOR_OPEN) {
            exit(passenger, e->id);
            e->occupancy--;
            e->barrier1 = 0; 
	    e->barrier2 = 0; 
	    riding=0;
	    pthread_barrier_wait((void*)&(e->barrier));
        }
    	pthread_mutex_unlock((void*)&(e->lock));
    } 
}


void elevator_ready(int elevator, int at_floor, 
        void(*move_direction)(int, int), 
        void(*door_open)(int), void(*door_close)(int)) {
    struct Elevator *e = &elevators[elevator]; 
    if(e->state == ELEVATOR_ARRIVED && e->target == e->current_floor) {
       
	door_open(elevator);
        e->state=ELEVATOR_OPEN;
	if(e->barrier2 == 0) {
	    pthread_barrier_wait((void*)&(e->elevatorBlock2));
	    e->barrier2 = 1;
	}
    }
    else if(e->state == ELEVATOR_OPEN && e->target == e->current_floor) {
	if(e->barrier1 == 0) {	    
	    pthread_barrier_wait((void*)&(e->elevatorBlock1));
	    e->barrier1 = 1;
	}
	if(e->target == e->to && e->current_floor == e->to) {
	   pthread_barrier_wait((void*)&(e->barrier));
	   e->target = -1;
	}
	door_close(elevator);
        e->state=ELEVATOR_CLOSED;
    }
    else {

        if(e->target != -1) {
		if(e->target > e->current_floor) {
		    e->direction = 1; 
		    e->current_floor=at_floor+e->direction;
		    move_direction(elevator, e->direction);
		}
		else if(e->target < e->current_floor) {
		    e->direction = -1; 
		    e->current_floor=at_floor+e->direction;
		    move_direction(elevator, e->direction); 
		}
		else {
		    e->state = ELEVATOR_ARRIVED;
		}
	}
	else { 
		if(at_floor == 0 || at_floor == FLOORS-1) 
			e->direction *= -1;
        	move_direction(elevator,e->direction);
        	e->current_floor=at_floor+e->direction;
	}
    }
}


