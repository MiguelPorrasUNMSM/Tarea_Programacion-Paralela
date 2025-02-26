#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int N = 1000; // Dimensión de las matrices

// Función que realiza la multiplicación en un rango de filas (versión paralela)
void multiplyRange(const vector<vector<double>>& A,
                   const vector<vector<double>>& B,
                   vector<vector<double>>& C,
                   int start, int end) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Función para la multiplicación secuencial
void multiplySequential(const vector<vector<double>>& A,
                        const vector<vector<double>>& B,
                        vector<vector<double>>& D) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            D[i][j] = sum;
        }
    }
}

int main() {
    // Inicialización de matrices A y B con números aleatorios en el rango de 0 a 10
    vector<vector<double>> A(N, vector<double>(N));
    vector<vector<double>> B(N, vector<double>(N));
    vector<vector<double>> C(N, vector<double>(N, 0.0)); // Para la versión paralela
    vector<vector<double>> D(N, vector<double>(N, 0.0)); // Para la versión secuencial

    // Configuración del generador de números aleatorios
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 10.0);  // Números aleatorios de 0 a 10

    // Llenar las matrices A y B con valores aleatorios
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = dis(gen);
            B[i][j] = dis(gen);
        }
    }

    // Multiplicación secuencial y medición del tiempo
    auto start_seq = high_resolution_clock::now();
    multiplySequential(A, B, D);
    auto end_seq = high_resolution_clock::now();
    double time_seq = duration_cast<duration<double>>(end_seq - start_seq).count();

    // Multiplicación paralela y medición del tiempo
    unsigned int numThreads = thread::hardware_concurrency();
    if(numThreads == 0)
        numThreads = 4; // Valor por defecto si no se detectan hilos disponibles
    vector<thread> threads;
    int rowsPerThread = N / numThreads;
    int start_row = 0;

    auto start_par = high_resolution_clock::now();
    for (unsigned int t = 0; t < numThreads; t++) {
        int end_row = (t == numThreads - 1) ? N : start_row + rowsPerThread;
        threads.push_back(thread(multiplyRange, cref(A), cref(B), ref(C), start_row, end_row));
        start_row = end_row;
    }
    for (auto &th : threads) {
        th.join();
    }
    auto end_par = high_resolution_clock::now();
    double time_par = duration_cast<duration<double>>(end_par - start_par).count();

    // Cálculo del speedup
    double speedup = time_seq / time_par;

    // Impresión de los tiempos y del speedup
    cout << "Tiempo secuencial: " << time_seq << " segundos" << endl;
    cout << "Tiempo paralelo: " << time_par << " segundos" << endl;
    cout << "Speedup: " << speedup << endl;

    return 0;
}
