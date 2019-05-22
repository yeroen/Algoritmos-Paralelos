#include <stdio.h>
#include <cuda_runtime.h>

__global__
void vecAddKernel(float* A, float* B, float* C, int n)
{
  int i = blockDim.x*blockIdx.x + threadIdx.x;
  if(i<n) C[i] = A[i] + B[i];
}

void vecAdd(float* A, float* B, float* C, int n)
{
  int size = n * sizeof(float);
  float *d_A, *d_B, *d_C;

  cudaMalloc((void **) &d_A, size);
  cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
  cudaMalloc((void **) &d_B, size);
  cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
  
  cudaMalloc((void **) &d_C, size);

  vecAddKernel<<<ceil(n/256.00), 256>>>(d_A, d_B, d_C, n);

  cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);

  for(int i = 0; i < n; ++i)
    printf("C[%d] = %f\n",i,C[i]);

  cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);

}

int main()
{
  int n = 10;
  float* A, *B, *C;
  A = (float*)malloc(n*sizeof(float));
  B = (float*)malloc(n*sizeof(float));
  C = (float*)malloc(n*sizeof(float));
  for(int i = 0; i < n; ++i)
  {
    A[i]= 1;
    B[i]= 1;
    C[i]= 0;  
  }
  vecAdd(A,B,C,n); 
}
