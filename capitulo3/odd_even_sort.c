#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 10

void init(int* data, int rank) {
  srand(rank);
  for (int i = 0; i < N; i++) { data[i] = rand( ) % 100; }
}

void print(int* data, int rank) {
  printf("Process %d: ");
  for (int i = 0; i < N; i++) { printf("%d ", data[i]); }
  printf("\n");
}

int cmp(const void* ap, const void* bp) {
  int a = * ((const int*) ap);
  int b = * ((const int*) bp);

  if (a < b) { return -1; } 
  else if (a > b) { return 1; } 
  else { return 0; }
}

int max_index(int* data) {
  int max = data[0], maxi = 0;
  for (int i = 1; i < N; i++) {
    if (data[i] > max) {
      max = data[i];
      maxi = i;
    }
  }
  return maxi;
}

int min_index(int* data) {
  int min = data[0], mini = 0;
  for (int i = 1; i < N; i++) {
    if (data[i] < min) {
      min = data[i];
      mini = i;
    }
  }
  return mini;
}

void odd_even_sort(int* data, int rank, int size) {
  int other[N];
  for (int i = 0; i < size; i++) {
    qsort(data, N, sizeof(int), &cmp);
    int partener;
    if (i % 2 == 0) {
      if (rank % 2 == 0) {
        partener = rank + 1;
      } else {
        partener = rank - 1;
      }
    } else {
      if (rank % 2 == 0) {
        partener = rank - 1;
      } else {
        partener = rank + 1;
      }
    }
    if (partener < 0 || partener >= size) {
      continue;
    }
    if (rank % 2 == 0) {
      MPI_Send(data, N, MPI_INT, partener, 0, MPI_COMM_WORLD);
      MPI_Recv(other, N, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(other, N, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(data, N, MPI_INT, partener, 0, MPI_COMM_WORLD);
    }
    if (rank < partener) {
      while (1) {
        int mini = min_index(other);
        int maxi = max_index(data);
        if (other[mini] < data[maxi]) {
          int temp = other[mini];
          other[mini] = data[maxi];
          data[maxi] = temp;
        } else {
          break;
        }
      }
    } else {
      while (1) {
        int maxi = max_index(other);
        int mini = min_index(data);
        if (other[maxi] > data[mini]) {
          int temp = other[maxi];
          other[maxi] = data[mini];
          data[mini] = temp;
        } else {
          break;
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  int rank, size;
  int data[N];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  init(data, rank);
  if(rank == 0){
    print(data, rank);
  }
  odd_even_sort(data, rank, size);
  print(data, rank);
  MPI_Finalize( );
  return 0;
}
