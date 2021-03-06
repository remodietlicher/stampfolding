#include <iostream>
#include <stdlib.h>
#include "mpi.h"

#include "domino.h"
#include "timer.h"
#include "mpi_stamp.h"

void mpi_main(int argc, char *argv[])
{
	int n_stamps, x, n_foldings_i, n_foldings;
	int rank, size;
	
	n_stamps = atoi(argv[1]);
	n_foldings = 0;
	double t1, t2;
	
	MPI::Init();
	t1 = MPI_Wtime();
	rank = MPI::COMM_WORLD.Get_rank();
	size = MPI::COMM_WORLD.Get_size();
	
	// do not allow this right now
	if(size < n_stamps-1){
		if(rank == 0)
			std::cout << "not enough processes spawned: (" << size << ", required " << n_stamps-1 << ")." << std::endl;
		MPI::Finalize();
		return;
	}
	  
	if(rank <= n_stamps-1)
	  n_foldings_i = calculate_foldings(rank+1, n_stamps);
	
	//std::cout << "Process " << rank << " calculated " << n_foldings_i <<
	//" possible Foldings!" << std::endl;
	
	
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
		std::cout << "Total number of foldings: " << n_stamps*n_foldings << std::endl;
		t2 = MPI_Wtime();
		std::cout << "total mpi time: " << t2 - t1 << " s" << std::endl;
	}	
	  
	MPI::Finalize();
}

void local_main_level(int argc, char *argv[]){
	timer t;
	t.start();
	int n_foldings, n_stamps;
	int level = 2;
	int setup[2];
	
	int n_foldings_i = 0;
	n_foldings = 0;
	n_stamps = atoi(argv[1]);
	
	int job_id = 0;
	int x_level_1, x_level_2;
	
	bool job_available = make_job(n_stamps, &job_id, &x_level_1, &x_level_2);
	while(job_available){
		setup[0] = x_level_1;
		setup[1] = x_level_2;
		//printf("calculating folding for setup: [%d, %d] \n", x_level_1, x_level_2);
		n_foldings_i = calculate_foldings(level, setup, n_stamps);
		printf("level 2 foldings: %d \n", n_foldings_i);
		job_available = make_job(n_stamps, &job_id, &x_level_1, &x_level_2);
	}
	
	std::cout << "calculated " << n_foldings << " foldings for branch level 0" << std::endl;
	std::cout << "therefore the total number of foldings is: " << n_stamps*n_foldings << std::endl;
	t.print_time();
}

void local_main(int argc, char *argv[]){
	timer t;
	t.start();
	int n_foldings, n_stamps;
	
	int n_foldings_i;
	n_foldings = 0;
	n_stamps = atoi(argv[1]);
	
	for(int i=0; i<n_stamps-1; i++){
		n_foldings_i = calculate_foldings(i+1, n_stamps);
		n_foldings += n_foldings_i;
		std::cout << "branch " << i << " has: " << n_foldings_i << std::endl;
	}
	
	std::cout << "calculated " << n_foldings << " foldings for branch level 0" << std::endl;
	std::cout << "therefore the total number of foldings is: " << n_stamps*n_foldings << std::endl;
	t.print_time();
}

int main(int argc, char *argv[]){
	if(argc == 3 && strcmp(argv[2], "mpi_old")==0)
		mpi_main(argc, argv);
	else if(argc == 3 && strcmp(argv[2], "level")==0)
		local_main_level(argc, argv);
	else if(strcmp(argv[0], "./stampfolding")==0 && argc==2){
		local_main(argc, argv);
	}
	else if(strcmp(argv[0], "stampfolding")==0 && argc==2){
		mpi_stamp(argc, argv);
	}
	else
		std::cout << "invalid call of 'stampfolding'. Usage: '...stampfolding N_STAMPS'" << std::endl;
	
	return 0;
}
