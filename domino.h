#ifndef DOMINO_H

#include <vector>
#include <utility>
#include <iostream>
#include <stdio.h>

int calculate_foldings(int x, int n_stamps);
bool intersects(int current_pos, int prev_pos, std::vector<std::pair<int, int> >& sealed_intervals, int parity);
void make_order(int value1, int value2, int& a, int& b);

#endif //DOMINO_H