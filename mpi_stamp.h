#ifndef MPI_STAMP_H

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <mpi.h>

#include "domino.h"

void run_master(int rank, int size, int n_stamps);
void run_worker(int rank, int size, int n_stamps);

//! calculate the number of stamps using MPI
void mpi_stamp(int argc, char *argv[]);

#endif // MPI_STAMP_H