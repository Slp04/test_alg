#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <cmath>
#include <mpi.h>

#include <sstream>

#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char** argv)
{ 
    int rank, size;
    ifstream fin;
    ofstream fout;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int N;
    if(rank == size-1)
    {
        fin.open("mtrx.txt");
        fout.open("res_par.txt");
        
        fin >> N;
    }
    
    MPI_Bcast(&N, 1, MPI_INT, size-1, MCW);
    
    //int *matrix;
    int *init_matrix = new int[N*N];
    int *buf = new int[N*N];
    int *send_steps = new int[size];
    //int *displs = new int[size];
    int step = N / size;
    int rm = N % size;
    
    if(rank == size-1)
    {
        //matrix = new int[N*N];
        for( int i = 0; i < N*N; i++)
            fin >> init_matrix[i];
        
        for (int i = 0; i < size; i++)
        {
            send_steps[i] = step;
            if(i == size -1)
                send_steps[i] += rm;
        }
        buf[0] = sqrt(init_matrix[0]);
    }
    
    MPI_Bcast(send_steps, size, MPI_INT, size-1, MCW);
    MPI_Bcast(init_matrix, N*N, MPI_INT, size-1, MCW);
    MPI_Bcast(buf, N*N, MPI_INT, size-1, MCW);
    
    //int str = N * send_steps[rank];
    int str = rank * send_steps[rank];
    
    stringstream ss;
    ss << rank;
    string r_str = ss.str();
    ofstream file_1("rz_ch_" + r_str + ".txt");
    
    /*for (int i = str, i_tmp = str * N; i < str + send_steps[rank]; ++i, i_tmp += N)
    {
        file_1 << "i = " << i << "; " << i << " < " << N << '\n';
        for (int j = 0; j <= i; ++j)
        {
            file_1 << "j = " << j << "; " << j << " <= " << i << '\n';
            double sum = 0.0;
            if (j == i)
            {
                file_1 << "j == i\n";
                for (int k = 0; k < j; k++)
                {
                    sum += buf[j * N + k] * buf[j * N + k];
                    file_1 << "k = " << k << "; " << k << " < " << j << '\n';
                    file_1 << "sum = " << sum << "buf[j][k] = " << buf[j * N + k] << '\n';
                }
                buf[j * N + j] = sqrt(init_matrix[j * N + j] - sum);
                MPI_Bcast(&buf[j * N + j], 1, MPI_INT, rank, MCW);
                file_1 << "buf[j * N + j] = " << buf[j * N + j] << " = " << "sqrt( " << init_matrix[j * N + j] << " - " << sum << " )" << '\n';
            }
            else
            {
                file_1 << "j != i\n";
                for (int k = 0; k < j; k++)
                {
                    sum += buf[i_tmp + k] * buf[j * N + k];
                    file_1 << "k = " << k << "; " << k << " < " << j << '\n';
                    file_1 << "sum = " << sum << "buf[i_tmp + k] = " << buf[i_tmp + k] << "buf[j * N + k] = " << buf[j * N + k] << '\n';
                }
                buf[i_tmp + j] = (init_matrix[i_tmp + j] - sum) / buf[j * N + j];
                MPI_Bcast(&buf[i_tmp + j], 1, MPI_INT, rank, MCW);
                file_1 << "buf[i_tmp * N + j] = " << buf[i_tmp + j] << " = " << "( " << init_matrix[i_tmp + j] << " - " << sum << " )" << " / " << buf[j * N + j] << '\n';
            }
        }
    }*/
    
    for (int i = 1; i < N; i++)
    {
        file_1 << "i = " << i << "; " << i << " < " << N << '\n';
        //for (int j = 0; j <= i; j++)
        for (int j = rank * step; j < (rank + 1) * step ; j++)
        {
            cout << "-------buf[4] = --------" << buf[4] << "rank = " << rank << '\n';
            file_1 << "j = " << j << "; " << j << " < " << (rank + 1) * step << '\n';
            double sum = 0.0;
            if(i == j)
            {
                file_1 << "i == j\n";
                for (int k = 0; k < j; k++)
                {
                    file_1 << "-------buf[4] = --------" << buf[4] << '\n';
                    sum += buf[i * N + k] * buf[i * N + k];
                    file_1 << "k = " << k << "; " << k << " <= " << j << '\n';
                    file_1 << "for1: buf[i * N + k] = " << i * N + k << '\n';
                    file_1 << "sum = " << sum << " buf[i * N + k] = " << buf[i * N + k] << '\n';
                }
                
                file_1 << "buf[i * N + i] = " << i * N + i << '\n';
                buf[i * N + i] = sqrt(init_matrix[i * N + i] - sum);
                file_1 << "buf[i * N + i] = " << buf[i * N + i] << " = " << "sqrt( " << init_matrix[i * N + i] << " - " << sum << " )" << '\n';
                //MPI_Bcast(&buf[i * N + i], 1, MPI_INT, rank, MCW);
                MPI_Allgather(buf, 1, MPI_INT, buf, 1, MPI_INT, MCW);
            }
            else
            {
                file_1 << "i != j\n";
                    for (int k = 0; k < j; k++)
                    {
                        sum += buf[i * N + k] * buf[j * N + k];
                        file_1 << "k = " << k << "; " << k << " <= " << j << '\n';
                        file_1 << "for2: buf[i * N + k] = " << i * N + i << " buf[j * N + k] = " << j * N + k << '\n';
                        file_1 << "sum = " << sum << " buf[i * N + k] = " << buf[i * N + k] << " buf[j * N + k] = " << buf[j * N + k] << '\n';
                    }
                    file_1 << "buf[i * N + j] = " << i * N + j << " init_matrix[j * N + i] = " << i * N + j << '\n';
                    buf[i * N + j] = (init_matrix[i * N + j] - sum) / buf[j * N + j];
                    file_1 << "buf[i * N + j] = " << buf[i * N + j] << " = " << "( " << init_matrix[i * N + j] << " - " << sum << " )" << " / " << buf[i * N + i] << '\n';
                    //MPI_Bcast(&buf[i * N + j], 1, MPI_INT, rank, MCW);
                    MPI_Allgather(buf, 1, MPI_INT, buf, 1, MPI_INT, MCW);
            }
            file_1 << "\n********\n\n";
        }
    }
    
    
    
    
    
    
    /*for (int i = 1; i < N; i++)
    //for (int i = str, i_tmp = str * N; i < str + send_steps[rank]; i++, i_tmp += N)
    {
        file_1 << "i = " << i << "; " << i << " < " << N << '\n';
        //for (int j = 0; j <= i; j++)
        for (int j = rank * step; j < (rank + 1) * step ; j++)
        {
            file_1 << "j = " << j << "; " << j << " < " << (rank + 1) * step << '\n';
            if(j <= i)
            {
                if(j != 0)
                {
                file_1 << "j <= i\n";
                double sum = 0.0;
                for (int k = 0; k < j; k++)
                {
                    sum += buf[i * N + k] * buf[i * N + k];
                    file_1 << "k = " << k << "; " << k << " <= " << j << '\n';
                    file_1 << "for1: buf[i * N + k] = " << i * N + k << '\n';
                    file_1 << "sum = " << sum << " buf[i * N + k] = " << buf[i * N + k] << '\n';
                }
                
                file_1 << "buf[i * N + i] = " << i * N + i << '\n';
                buf[i * N + i] = sqrt(init_matrix[i * N + i] - sum);
                file_1 << "buf[i * N + i] = " << buf[i * N + i] << " = " << "sqrt( " << init_matrix[i * N + i] << " - " << sum << " )" << '\n';
                MPI_Bcast(&buf[i * N + i], 1, MPI_INT, rank, MCW);
                }
                else
                {
                    double sum = 0.0;
                    for (int k = 0; k < j; k++)
                    {
                        sum += buf[i * N + k] * buf[j * N + k];
                        file_1 << "k = " << k << "; " << k << " <= " << j << '\n';
                        file_1 << "for2: buf[i * N + k] = " << i * N + i << " buf[j * N + k] = " << j * N + k << '\n';
                        file_1 << "sum = " << sum << " buf[i * N + k] = " << buf[i * N + k] << " buf[j * N + k] = " << buf[j * N + k] << '\n';
                    }
                    file_1 << "buf[i * N + j] = " << i * N + j << " init_matrix[j * N + i] = " << j * N + i << '\n';
                    buf[i * N + j] = (init_matrix[i * N + j] - sum) / buf[j * N + j];
                    file_1 << "buf[i * N + j] = " << buf[i * N + j] << " = " << "( " << init_matrix[i * N + j] << " - " << sum << " )" << " / " << buf[i * N + i] << '\n';
                    MPI_Bcast(&buf[i * N + j], 1, MPI_INT, rank, MCW);
                 }
            }
            file_1 << "\n********\n\n";*/
            
            /*double sum = 0.0;
            if (j == i)
            {
                for (int k = 0; k < j; k++)
                {
                    sum += buf[j * N + k] * buf[j * N + k];
                }
                buf[j * N + j] = sqrt(init_matrix[j * N + j] - sum);
                MPI_Bcast(&buf[j * N + j], 1, MPI_INT, rank, MCW);
            }
            else
            {
                for (int k = 0; k < j; ++k)
                {
                    sum += buf[i_tmp + k] * buf[j * N + k];
                }
                //L[i][j] = (A[i][j] - sum) / L[j][j];
                buf[i_tmp + j] = (init_matrix[i_tmp + j] - sum) / buf[j * N + j];
                MPI_Bcast(&buf[i_tmp + j], 1, MPI_INT, rank, MCW);
            }*/
        //}
    //}
    
    file_1.close();
    
    MPI_Barrier(MCW);
    
    if (rank == size - 1)
    {
        for(int i = 0; i < N*N; i++)
        {
            fout << buf[i] << ' ';
            if((i+1)%N == 0)
                fout << endl;
        }
        
        fin.close();
        fout.close();
    }
  
    delete[] init_matrix;
    delete[] buf;
    delete[] send_steps;
  
    MPI_Finalize();
    return 0;
    
}

