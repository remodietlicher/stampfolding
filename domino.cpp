#include "domino.h"

//! return true, if the interval [current_pos, prev_pos] intersects any of the sealed_intervals
//! parity: 0 for even numbers, 1 for odd numbers. Note that even/odd stones only care about odd/even intervals.
bool intersects(int current_pos, int prev_pos, std::vector<std::pair<int, int> >& sealed_intervals, int parity)
{
	bool out = false;
	int a1, a2, b1, b2;
	// flip parity since even stones care about odd intervals.
	for(int i=parity; i<sealed_intervals.size(); i+=2){
		make_order(current_pos, prev_pos, a1, b1);
		make_order(sealed_intervals[i].first, sealed_intervals[i].second, a2, b2);
		
		bool inter = (a1-a2)*(b1-b2) > 0 && (a1-b2)*(b1-a2) < 0;
		
		//printf("testing for intersection: (%d, %d) vs. [%d, %d]. Outcome: %d \n", a1, b1, a2, b2, inter);
		
		if((a1-a2)*(b1-b2) > 0 && (a1-b2)*(b1-a2) < 0)
			out = true;
	}
	
	return out;
}

//! assign values value1 and value2 such that a contains the smaller, and b the larger value.
void make_order(int value1, int value2, int& a, int& b){
	if(value1 <= value2){
		a = value1;
		b = value2;
	} else {
		a = value2;
		b = value1;
	}
}

//! calculate the number of foldings in the branch specified by the starting stone at x
int calculate_foldings(int x, int n_stamps){
	int n_foldings = 0;
	// map: stone_nr -> sealed interval. We label the interval by its first stone. (stone 0 -> first interval)
	std::vector<std::pair<int, int> > sealed_intervals(n_stamps-1, std::make_pair(0, 0));
	std::vector<bool> occupied(n_stamps, false);						// keep track of the occupied fields
	std::vector<int> board(n_stamps, -1);								// map: stone_nr -> element in "occupied"
	
	// we start with stone 0 (stone_nr==0) at "0" and stone 1 (stone_nr==0) at "x"
	board[0] = 0;
	occupied[0] = true;	
	board[1] = x;
	occupied[x] = true;
	
	sealed_intervals[1] = std::make_pair(board[0], board[1]);
	
	unsigned int stone_nr=2;						// as 0 & 1 is already on the board, we start with nr 2
	
	board[stone_nr] = 1;							// we will try to set the first stone to 1 since 0 is always occupied
	
	// we calculate all foldings for which we do not have to move stone 0.
	while(stone_nr > 1){
		bool setting_stones = true;
		while(setting_stones)
		{
			// search for the first legal position right to current_pos:
			// increment current_pos as long as:
			// current_pos is within the legal range AND
			// the position is occupied OR forbidden by intersection rule.
			while(board[stone_nr]<n_stamps && 
				  (occupied[board[stone_nr]] || intersects(board[stone_nr], board[stone_nr-1], sealed_intervals, stone_nr%2))){
				board[stone_nr]++;
			}
			// if there is no legal position left, we are done
			if(board[stone_nr] >= n_stamps)
				setting_stones = false;
			// if there was a legal position and we're handling the last stone, we are done
			else if(stone_nr == n_stamps-1){
				setting_stones = false;
				n_foldings++;
			}
			// else a stone is set, information is updated and we continue setting stones
			else
			{
				occupied[board[stone_nr]] = true;

				sealed_intervals[stone_nr] = std::make_pair(board[stone_nr-1], board[stone_nr]);	// handle sealed intervals
									
				stone_nr++;																// take the next stone...
				
				board[stone_nr] = 1;													// and set it to 1
			}
		}
		
		stone_nr--;											// take the previous stone...
		occupied[board[stone_nr]] = false;					
		board[stone_nr]++;									// ... move it one to the right
		
		sealed_intervals[stone_nr] = std::make_pair(0, 0);		// delete its spanned interval
		
	}
	
	return n_foldings;
}