#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int thread_count;

void Odd_even(int a[], int n){
  int phase, i, tmp;
#pragma omp parallel num_threads(thread_count) default(none) shared(a,n) private(i,tmp,phase)
  for(phase = 0; phase < n; phase++){
    if(phase % 2 == 0)
      #pragma omp for
      for(i = 1; i < n; i+=2){
	tmp = a[i-1];
	a[i-1] = a[i];
	a[i] = tmp;
      }
    else
      #pragma omp for
      for(i = 1; i < n-1; i+=2){
	tmp = a[i+1];
	a[i+1] = a[i];
	a[i] = tmp;
      }
  }
}

int main(int argc, char* argv[]){
  thread_count = strtol(argv[1], NULL, 10);
  int n, *A, i;
  double elapsed;
  struct timeval begin, end;
  scanf("%d", &n);
  A =  malloc(n*sizeof(int));
  for(i = 0; i < n; i++) A[i] = n-i-1;
  gettimeofday(&begin, NULL);
  Odd_even(A,n);
  gettimeofday(&end, NULL);
  elapsed = ((end.tv_sec - begin.tv_sec) * 1000000u + end.tv_usec - begin.tv_usec) / 1.e6;
  printf("%lf\n", elapsed);
  //for(i = 0; i < n; i++) printf("%d ", A[i]);
  //printf("\n");
  return 0;
}


