#ifndef DOMINO_H

#include <vector>
#include <utility>
#include <iostream>
#include <stdio.h>

bool make_job(int n_stamps, int *job_id, int *x1, int *x2);
int calculate_foldings(int x, int n_stamps);
int calculate_foldings(int level, int* setup, int n_stamps);
bool intersects(int current_pos, int prev_pos, std::vector<std::pair<int, int> >& sealed_intervals, int parity);
void make_order(int value1, int value2, int& a, int& b);

#endif //DOMINO_H