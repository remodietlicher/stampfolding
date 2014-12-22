#include "mpi_stamp.h"


//! run the process that distributes jobs and collects calculated data.
void run_master(int rank, int size, int n_stamps){
	long long n_foldings = 0;
	double t1, t2;
	int x_level_1, x_level_2;
	t1 = MPI_Wtime();
	
	int level_1_max = (n_stamps-1)/2+(n_stamps-1)%2;			// we only need to consider half of them
	
	int n_jobs = level_1_max*(n_stamps-1)-level_1_max;
	
	int job_id_max = level_1_max*(n_stamps) + (n_stamps-1);
	
	std::vector<int> job_data(job_id_max+1, -1);				// initialize data array. unusable job_ids are -1
	
	int job_buffer[3];											// we send jobs: x_level_1, x_level_2 and job_id.
	int work_buffer[2];											// buffer to receive work: n_foldings_i, job_id.
	
	// provide all workers with a job
	int job_id = 0;
	int job_cnt = 0;
	int open_jobs = 0;
	bool job_available = true;
	for(int p=1; p<size; p++){
		job_available = make_job(n_stamps, &job_id, &x_level_1, &x_level_2);
		job_buffer[0] = x_level_1;
		job_buffer[1] = x_level_2;
		job_buffer[2] = job_id;
		if(job_available){
			job_cnt++;
			open_jobs++;
			//printf("MASTER> sending job %d/%d: [%d,%d] (id: %d/%d)) to process %d \n", job_cnt, n_jobs, x_level_1, x_level_2, job_id, job_id_max, p);
		}
		MPI::COMM_WORLD.Send(job_buffer, 3, MPI::INT, p, 0);
	}
	
	while(open_jobs > 0){
		//printf("MASTER> processing jobs. Open: %d, Sent: %d (out of %d) \n", open_jobs, job_cnt, n_jobs);
		MPI::Status status;
		MPI::COMM_WORLD.Recv(work_buffer, 2, MPI::INT, MPI_ANY_SOURCE, 0, status);
		//printf("MASTER> received work (%d) done by process %d \n", work_buffer[0], status.Get_source());
		printf("MASTER> received work: %d/%d \n", job_cnt-open_jobs+1, n_jobs);
		job_data[work_buffer[1]] = work_buffer[0];
		open_jobs--;
		
		job_available = make_job(n_stamps, &job_id, &x_level_1, &x_level_2);
		job_buffer[0] = x_level_1;
		job_buffer[1] = x_level_2;
		job_buffer[2] = job_id;
		if(job_available){
			job_cnt++;
			open_jobs++;
			//printf("MASTER> sending job %d/%d: [%d,%d] (id: %d/%d)) to process %d \n", job_cnt, n_jobs, x_level_1, x_level_2, job_id, job_id_max, status.Get_source());
		}
		// we send the finished mark anyway
		MPI::COMM_WORLD.Send(job_buffer, 3, MPI::INT, status.Get_source(), 0);
	}
	
	//printf("MASTER> Open jobs: %d. Finalizing work... \n", open_jobs);
	n_foldings = 0;
	for(int i=0; i<job_id_max+1; i++){
		if(job_data[i] >= 0) {
			std::cout << job_data[i] << " ";
			n_foldings += job_data[i];
		}
	}
	std::cout << std::endl;
	
	n_foldings *= 2;
	if(n_stamps%2==0) {
		int i = job_id_max;
		int del_cnt = 0;
		while(del_cnt < n_stamps-2){
			if(job_data[i] >= 0){
				del_cnt++;
				n_foldings -= job_data[i];
			}
			i--;
		}
	}
	
	n_foldings *= n_stamps;
	printf("calculated %d foldings \n", n_foldings);
	t2 = MPI_Wtime();
	std::cout << "total mpi time: " << t2 - t1 << " s" << std::endl;
}

//! run the process that receives job information and calculates number of foldings.
void run_worker(int rank, int size, int n_stamps){
	int job_buffer[3];
	int work_buffer[2];
	int n_foldings_i = rank;
	int x_level[2];
	int job_id;
	bool work_to_do = true;
	while(work_to_do){
		MPI::COMM_WORLD.Recv(job_buffer, 3, MPI::INT, 0, 0);
		//printf("PROCESS%d> received job: [%d,%d] \n", rank, job_buffer[0], job_buffer[1]);
		x_level[0] = job_buffer[0];
		x_level[1] = job_buffer[1];
		job_id = job_buffer[2];
		
		// handle abort tag
		if(job_buffer[0] < 0){
			//printf("Process %d ran out of work.\n", rank);
			work_to_do = false;
			//printf("PROCESS%d> got abort signal. \n", rank);
		}
		else{
			// do work
			//printf("PROCESS%d> doing work... \n", rank);
			n_foldings_i = calculate_foldings(2, x_level, n_stamps);
			
			
			/*
			int t1 = MPI_Wtime();
			int t2 = MPI_Wtime();
			while(t2 -t1 < rank%3+2)
				t2 = MPI_Wtime();
			*/
			
			work_buffer[0] = n_foldings_i;
			work_buffer[1] = job_id;
			//printf("PROCESS%d> work is done. sending to master... \n", rank);
			MPI::COMM_WORLD.Send(work_buffer, 2, MPI::INT, 0, 0);
		}
	}
	//printf("PROCESS%d> no more work to do.\n", rank);
	
	
	/*
	int x, n_foldings_i;
	  
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
		*/
}

void mpi_stamp(int argc, char *argv[])
{
	int n_stamps, x, n_foldings_i, n_foldings;
	int rank, size;
	
	n_stamps = atoi(argv[1]);
	
	MPI::Init();
	rank = MPI::COMM_WORLD.Get_rank();
	size = MPI::COMM_WORLD.Get_size();
	
	if(rank == 0)
		run_master(rank, size, n_stamps);
	else
		run_worker(rank, size, n_stamps);
	
	
	MPI::Finalize();
}