/*#include <iostream>
#include <iomanip>
#include <cmath>
#include "mpi.h"*/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <cmath>
#include <mpi.h>

using namespace std;

struct timeval tv1, tv2, dtv;
void time_start();
double time_stop();

int main(int argc,char* argv[]) 
{
	int size,rank,t,N;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	ifstream fin;
        ofstream fout;
        
        if(rank == size-1)
        {
                fin.open("mtrx.txt");
                fout.open("res_par.txt");
        
                fin >> N;
        }
	
	MPI_Bcast(&N,1,MPI_INTEGER,size-1,MPI_COMM_WORLD);
	
	double matrix[N * N];

	double *result_L, temp;
	result_L=new double[N*N];
	for(int i = 0; i < N*N;++i)
		result_L[i] =  0.0;

        if(rank == size-1)
        {
                for( int i = 0; i < N*N; i++)
                        fin >> matrix[i];
                
                time_start();
        }

	MPI_Bcast(matrix,N*N,MPI_DOUBLE,size-1,MPI_COMM_WORLD);
	
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
	
	if(rank==size-1)
	{ 
		cout << "Time: " << time_stop() << " ms \n";
		for(int i = 0; i < N*N; i++)
                {
                        fout << result_L[i] << ' ';
                        if((i+1)%N == 0)
                                fout << endl;
                }
        
                fin.close();
                fout.close(); 
	}
	
	delete []result_L;
	MPI_Finalize();
	return EXIT_SUCCESS;
}

void time_start()
{
    gettimeofday(&tv1, NULL);
}

double time_stop()
{
    gettimeofday(&tv2, NULL);
    dtv.tv_sec = tv2.tv_sec - tv1.tv_sec;
    dtv.tv_usec = tv2.tv_usec - tv1.tv_usec;
    
    if(dtv.tv_usec < 0)
    {
        dtv.tv_sec--;
        dtv.tv_usec+=1000000;
    }
    return double((dtv.tv_sec*1000000+dtv.tv_usec)/1000.0);
}

