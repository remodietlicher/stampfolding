#include <iostream>
#include "mpi.h"

#include "domino.h"

#define N_STAMPS 6

void mpi_main()
{
  int n_stamps, x, n_foldings_i, n_foldings;
  int rank, size;

  n_stamps = N_STAMPS;
  n_foldings = 0;
  
  MPI::Init();
  rank = MPI::COMM_WORLD.Get_rank();
  size = MPI::COMM_WORLD.Get_size();
  
  // do not allow this right now
  if(size < n_stamps){
	  if(rank == 0)
		  std::cout << "not enough processes spawned: (" << size << ", required " << n_stamps << ")." << std::endl;
	  MPI::Finalize();
	  return;
  }

  //for later: only consider the case where size >= n_stamps at first
  bool* assigned = new bool[n_stamps];
  for(int i=0; i<n_stamps; i++)
    assigned[i] = false;
	  
  if(rank <= n_stamps)
	  n_foldings_i = calculate_foldings(x, n_stamps);

  std::cout << "Process " << rank << " calculated " << n_foldings_i <<
    " possible Foldings!" << std::endl;
  
  
  if(rank != 0){
	  MPI::COMM_WORLD.Send(&n_foldings_i, 1, MPI::INT, 0, 0);
  }
  else{
	  // add #foldings calculated by process 0
	  n_foldings += n_foldings_i;
	  std::cout << "getting " << n_foldings_i << " from process " << 0 << std::endl;
	  
	  // collect #foldings of other processes
	  for(int p=1; p<size; p++){
		  MPI::COMM_WORLD.Recv(&n_foldings_i, 1, MPI::INT, p, 0);
		  std::cout << "getting " << n_foldings_i << " from process " << p << std::endl;
		  n_foldings += n_foldings_i;
	  }
  }
  
  if(rank == 0)
	  std::cout << "Total number of foldings by Process " << rank << ": " << n_foldings << std::endl;
  	  
  MPI::Finalize();
}

void local_main(){
	int n_foldings, n_stamps;
	
	int n_foldings_i;
	n_foldings = 0;
	n_stamps = N_STAMPS;
	
	for(int i=0; i<n_stamps; i++){
		n_foldings_i = calculate_foldings(i, n_stamps);
		n_foldings += n_foldings_i;
		std::cout << "branch " << i << " has: " << n_foldings_i << std::endl;
	}
	
	std::cout << "calculated " << n_foldings << " foldings" << std::endl;
}

bool test_intersection()
{
	std::vector<std::pair<int, int> > intervals(3);
	
	intervals[0] = std::make_pair(0, 5);
	intervals[1] = std::make_pair(2, 3);
	intervals[2] = std::make_pair(7, 10);
	
	int a = 6;
	int b = 4;
	
	return intersects(a, b, intervals, 0);
}

int main(){
	//local_main();
	mpi_main();
	
	//std::cout << "outcome of 'test_intersection()': " << test_intersection() << std::endl;
	
	return 0;
}
