#ifndef TIMER_H

#include <time.h>
#include <stdio.h>

class timer
{	
public:
	clock_t start_time;
	clock_t round_time;
	
	timer()
	{}
	//! start the timer
	void start();
	//! get the total time in seconds (does not trigger a new round)
	float get_time();
	//! get the time since last round (triggers new round)
	float get_round();
	//! directly print total time
	void print_time();
	//! directly print round
	void print_round();
	
};

#endif // TIMER_H