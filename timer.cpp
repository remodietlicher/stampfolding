#include "timer.h"

void timer::start(){
	start_time = clock();
	round_time = start_time;
}
float timer::get_time(){
	clock_t t = clock() - start_time;
	return ((float)t)/CLOCKS_PER_SEC;
}
float timer::get_round(){
	clock_t t = clock() - round_time;
	round_time = clock();
	return ((float)t)/CLOCKS_PER_SEC;
}
void timer::print_time(){
	float t = get_time();
	printf("Total time: %.5f s\n", t);
}
void timer::print_round(){
	float t = get_round();
	printf("Round time: %.5f s \n", t);	
}