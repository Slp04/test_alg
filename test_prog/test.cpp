#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <mpi.h>
#include <sstream>

using namespace std;

void write_mtrx(const vector<vector<double>>& matrix, const string& filename)
{
    std::ofstream file(filename);
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file << matrix[i][j] << " ";
        }
        file << std::endl;
    }
}

vector<double> rz_ch(const vector<double>& A, const int n)
{
    std::vector<double> L(n*n, 0.0);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j <= i; ++j)
        {
            double sum = 0.0;
            if (j == i)
            {
                for (int k = 0; k < j; ++k)
                {
                    sum += L[j*n+k] * L[j*n+k];
                }
                L[j*n+j] = sqrt(A[j*n+j] - sum);
            }
            else
            {
                for (int k = 0; k < j; ++k)
                {
                    sum += L[i*n+k] * L[j*n+k];
                }
                L[i*n+j] = (A[i*n+j] - sum) / L[j*n+j];
            }
        }
    }

    return L;
}

int main(int argc, char** argv)
{
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ifstream file;
    int n;

    if (rank == size-1) {
        file.open("mtrx.txt");
        if (!file.is_open())
        {
            cout << "Файл не найден\n";
            return -1;
        }
        file >> n;
    }
    
    MPI_Bcast(&n, 1, MPI_INT, size - 1, MPI_COMM_WORLD);
    
    vector<vector<double>> matrix(n, vector<double>(n));
    
    if (rank == size-1)
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {   
                file >> matrix[i][j];
            }
        }
        file.close();
    }
    
    vector<double> localData(n * n / size);
    MPI_Scatter(matrix.data(), n * n / size, MPI_DOUBLE, localData.data(), n * n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    localData = rz_ch(localData, n);
    
    vector<double> resultData(n * n);
    MPI_Gather(localData.data(), n * n / size, MPI_DOUBLE, resultData.data(), n * n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == size-1)
    {
        vector<vector<double>> resultMatrix(n, vector<double>(n));
        for (int i = 0; i < size; ++i) 
        {
            for (int j = 0; j < n * n / size; ++j) 
            {
                resultMatrix[i * n / size + j / n][i * n / size + j % n] = resultData[i * n * n / size + j];
            }
        }
        stringstream ss;
        ss << rank;
        string r_str = ss.str();
        write_mtrx(resultMatrix, "result_par_" + r_str + ".txt");
    }
    
    MPI_Finalize();
    
    return 0;
}

