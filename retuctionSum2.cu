#include <stdio.h>
#define BLOCK_SIZE 128
#define BLOCK_SIZE_F 128.0

__global__
void sumRedKernel(float *A, int n){
  __shared__ float partialSum[BLOCK_SIZE*2];
  int i = (threadIdx.x + blockDim.x * blockIdx.x)*2;
  unsigned int t = threadIdx.x * 2;
  partialSum[t] = A[i];
  partialSum[t+1] = A[i+1];
  t = threadIdx.x;
  for(unsigned int stride = blockDim.x; stride > 0; stride /= 2){
    __syncthreads();
    if(t < stride && (t+stride) < n) partialSum[t] += partialSum[t+stride];
  }
  __syncthreads();
  if(threadIdx.x == 0){
    A[blockIdx.x] = partialSum[0];
  }
}

void sumRed(float* A, int n){
  int size = n*sizeof(float);
  float *d_A;

  cudaMalloc((void **) &d_A, size);
  cudaMemcpy(d_A,A,size,cudaMemcpyHostToDevice);

  sumRedKernel<<<ceil(n/(BLOCK_SIZE_F*2)), BLOCK_SIZE>>>(d_A,n);
  
  cudaMemcpy(A,d_A,size,cudaMemcpyDeviceToHost);

  cudaFree(d_A);
}


int main(){
  int n,i;
  float *h_A;
  scanf("%d", &n);
  h_A = (float*) malloc(n*sizeof(float));
  for(i = 0; i < n; i++){
    //scanf("%f", &h_A[i]);
    h_A[i] = 1;
  }
  while(n > 1){
     sumRed(h_A,n);
     n = ceil(n/(BLOCK_SIZE_F*2));
  }
  printf("%f", h_A[0]);
  printf("\n");
  return 0;
}
