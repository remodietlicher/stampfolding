#include "domino.h"

//! returns false if the parity rule is not satisfied (i.e. a stone should not be placed)
bool parity_rule(int n_stamps, int n_encl, int stone_nr){
	bool out;
	if(n_stamps%2 == 0)
		// do not allow odd stones to have an odd number of enclosed free spaces.
		// i.e. either the stone is even or the n_encl is even.
		out = ((stone_nr+1)%2 + (n_encl+1)%2) != 0;
	else
		// do not allow even stones to have an odd number of enclosed free spaces.
		// e.g. either the stone is odd, or the n_encl is even.
		out = (stone_nr%2 + (n_encl+1)%2) != 0;
	
	return out;
}

//! given x1_max, n_stamps and the last job_id
//! it calculates the next possible level 2 setup.
//! Example usage:
//! loop:
//! 	make_job
//! 	handle job
//! --> job_ids are handles automatically.
bool make_job(int n_stamps, int *job_id, int *x1, int *x2){
	int max = (n_stamps-1)/2+(n_stamps-1)%2;
	(*job_id)++;
	bool out = true;
	*x1 = (*job_id)/(n_stamps);
	*x2 = (*job_id)%(n_stamps);
	while(*x1 == *x2 || (*x2)*(*x1) == 0){
		(*job_id)++;
		*x1 = (*job_id)/(n_stamps);
		*x2 = (*job_id)%(n_stamps);
	}
	if(*x1 > max){
		// mark as not needed
		*x1 = -1;
		*x2 = -1;
		out = false;
	}
	return out;
}

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

//! calculate the number of foldings in the branch specified by setup and level.
int calculate_foldings(int level, int* setup, int n_stamps){
	int n_foldings = 0;
	// map: stone_nr -> sealed interval. We label the interval by its first stone. (stone 0 -> first interval)
	std::vector<std::pair<int, int> > sealed_intervals(n_stamps-1, std::make_pair(0, 0));
	std::vector<bool> occupied(n_stamps, false);						// keep track of the occupied fields
	std::vector<int> board(n_stamps, -1);								// map: stone_nr -> element in "occupied"
	
	board[0] = 0;														// we set branch 0, thats hard-coded
	occupied[0] = true;	
	
	bool legal_setup = true;
	for(int l=1; l<level+1; l++){
		board[l] = setup[l-1];
		if(intersects(board[l], board[l-1], sealed_intervals, l%2) || occupied[board[l]]) {
			legal_setup = false;
			printf("illegal setup: [%d, %d]", setup[0], setup[1]);
			break;
		}
		else {
			occupied[setup[l-1]] = true;
			sealed_intervals[l] = std::make_pair(board[l-1], board[l]);
		}
	}
	
	if(legal_setup) {
		unsigned int stone_nr=level+1;			// as level stones already on the board, we start with nr level+1
		board[stone_nr] = 1;					// we will try to set the first stone to 1 since we are on branch 0.
		
		// we calculate all foldings for which we do not have to move stone_nr level. E.g:
		// level 0 is where we do not move stone_nr 0,
		// level 5 is where we do not move stone_nr 5, etc.
		while(stone_nr > level){
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
				// calculate number of enclosed, free places for parity rule.
				int a, b;
				int n_encl = 0;
				make_order(board[stone_nr], board[stone_nr-1], a, b);
				for(int l=a+1; l<b; l++){
					n_encl += (((int)occupied[l])+1)%2;			// if not occupied: +1
				}
				
				// if there is no legal position left, we are done
				if(board[stone_nr] >= n_stamps)
					setting_stones = false;
				// if there was a legal position and we're handling the last stone, we are done
				else if(stone_nr == n_stamps-1){
					setting_stones = false;
					n_foldings++;
				}
				else if(!parity_rule(n_stamps, n_encl, stone_nr)){
					board[stone_nr]++;
				}
				else // else a stone is set, information is updated and we continue setting stones
				{
					
					occupied[board[stone_nr]] = true;

					sealed_intervals[stone_nr] = std::make_pair(board[stone_nr-1], board[stone_nr]);	// handle sealed intervals
										
					stone_nr++;																// take the next stone...
					
					board[stone_nr] = 1;													// and set it to 1
				}
			}
			
			stone_nr--;												// take the previous stone...
			occupied[board[stone_nr]] = false;					
			board[stone_nr]++;										// ... move it one to the right
			
			sealed_intervals[stone_nr] = std::make_pair(0, 0);		// delete its spanned interval
		}
	}
	else
		return n_foldings = -1;
	
	return n_foldings;
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