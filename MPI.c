#include "mpi.h"
#include <stdio.h>
#include "time.h"
#define AMOUNT 1000

int main (int argc, char *argv[]) {
  srand(time(NULL));
  int i, numproccess, rank, chunk;
  int max, medmax, remain;
  int list[AMOUNT];
  double start, stop;

  MPI_Status status;
  MPI_Init(&argc, &argv); // start the MPI
  start = MPI_Wtime();
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numproccess); // MPI_COMM_WORLD - number of total processors

  chunk = AMOUNT/numproccess;
  remain = AMOUNT % numproccess;

  if (rank == 0) {
    printf("Remain %d\n", remain);
    printf("Massive: \n");
    for (i=0; i<AMOUNT; i++) {
      list[i] = rand() % 10000 + 1;
      printf("%d ", list[i]);
    }

    // distribute
    for (i=1; i<numproccess; i++) {
      if (i < remain) {
        MPI_Send(&list[i*chunk], chunk + 1, MPI_INT, i, 1, MPI_COMM_WORLD);
      } else {
        MPI_Send(&list[i*chunk], chunk, MPI_INT, i /*processor #i*/, 1 /*tag - can use MPI_ANY_TAG*/, MPI_COMM_WORLD);
      }
    }
  } else {
    for (i=1; i<numproccess; i++) {
        if (i<remain) {
          MPI_Recv(list, chunk + 1, MPI_INT, 0 /*receive signal from processor 0*/, 1, MPI_COMM_WORLD, &status);
        } else {
          // wait to receive from send in overhead time
          MPI_Recv(list, chunk, MPI_INT, 0 /*receive signal from processor 0*/, 1, MPI_COMM_WORLD, &status);
        }
    }
  }
  // now each processor has its own chunk
  // find local max in processor 0
  medmax = list[0];
  for (i=0; i<chunk; i++) {
    if (medmax < list[i]) {
      medmax = list[i];
    }
  }

  if (rank == 0) {
    max = medmax;
    for (i=1; i < numproccess; i++) {
      MPI_Recv(&medmax,/*size of medmax*/ 1, MPI_INT,/*receive from any sender available*/ MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
      if (max < medmax) {
        max = medmax;
      }
    }
    printf("\nThe max is: %d\n", max);
  } else {
    MPI_Send(&medmax, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }

  stop = MPI_Wtime();
  if (rank == 0) {
      printf("The time is: %fs\n", stop - start);
  }

  MPI_Finalize(); // to end the parallel section
  return 0;
}
