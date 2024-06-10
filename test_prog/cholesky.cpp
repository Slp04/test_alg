#include <iostream>
#include <iomanip>
#include <cmath>
#include "mpi.h"
#define N 5
using namespace std;

void outMatrix(double *matrix){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++)
			cout << setw(10) << matrix[i*N+j];
		cout << endl; 
	}
}

int main(int argc,char* argv[]) {
	int size,rank,t; 
	double matrix[N * N] = { 112, 39, 110, 37, 125,
			   	  39, 74,  48, 19, 60,
			  	 110, 48, 127, 44, 151,
			   	  37, 19,  44, 16, 53,
			  	 125, 60, 151, 53, 224};

	double *result_L, temp;
	result_L=new double[N*N];
	for(int i = 0; i < N*N;++i)
		result_L[i] =  0.0;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Bcast(matrix,N*N,MPI_DOUBLE,0,MPI_COMM_WORLD);
	
	for(int ind = 0; ind < N; ind++)  {
		if (ind % size == rank) {
			for (int i=0; i<ind+1; i++) { 
				if (i<ind) { 
					temp = matrix[i*N+ind];
					for (int k=0; k<ind; k++)
						temp -= result_L[ind*N+k] * result_L[i*N+k];
					result_L[ind*N+i] = temp / result_L[i*N+i];
				}
				if (i==ind) {
					temp = matrix[i*N+i];
					for (int k=0; k<i; k++)
						temp -= result_L[ind*N+k]*result_L[ind*N+k];
					result_L[ind*N+ind] = sqrt(temp); 
				} 		
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&result_L[ind*N], ind+1, MPI_DOUBLE, ind%size, MPI_COMM_WORLD);
    	}
	
	if(rank==0){ 
		outMatrix(result_L); 
	}
	delete []result_L;
	MPI_Finalize();
	return EXIT_SUCCESS;
}
