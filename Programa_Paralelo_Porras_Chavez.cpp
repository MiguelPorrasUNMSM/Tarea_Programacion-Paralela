#include <iostream>
#include <vector>
#include <thread>

// Función que realiza la multiplicación en un rango de filas
// Aquí se aplica el concepto de Partition: se asigna a cada hilo un rango (subconjunto) de filas para procesar.
void multiplyRange(const std::vector<std::vector<double>>& A,
                   const std::vector<std::vector<double>>& B,
                   std::vector<std::vector<double>>& C,
                   int start, int end, int N) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;  // Communication: Cada hilo escribe en su sección exclusiva de la matriz C sin interferir con otros.
        }
    }
}

int main() {
    int N;
    std::cout << "Ingresa la dimension de las matrices cuadradas: ";
    std::cin >> N;
    
    // Inicialización de las matrices A, B y C (la matriz C se inicializa en cero)
    std::vector<std::vector<double>> A(N, std::vector<double>(N));
    std::vector<std::vector<double>> B(N, std::vector<double>(N));
    std::vector<std::vector<double>> C(N, std::vector<double>(N, 0.0));
    
    // Entrada de datos para la matriz A
    std::cout << "Ingresa los valores de la matriz A:" << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << "A[" << i << "][" << j << "]: ";
            std::cin >> A[i][j];
        }
    }
    
    // Entrada de datos para la matriz B
    std::cout << "Ingresa los valores de la matriz B:" << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << "B[" << i << "][" << j << "]: ";
            std::cin >> B[i][j];
        }
    }
    
    // Mapping: Se determina el número de hilos disponibles y se decide la forma de asignar el trabajo.
    unsigned int numThreads = std::thread::hardware_concurrency();
    if(numThreads == 0)
        numThreads = 4; // Valor por defecto si no se detecta la cantidad de hilos disponibles.
    std::vector<std::thread> threads;
    int rowsPerThread = N / numThreads;
    int start = 0;
    
    // Partition y Mapping:
    // Se distribuyen las filas de la matriz C entre los hilos, asignando a cada hilo un rango específico.
    for (unsigned int t = 0; t < numThreads; t++) {
        int end = (t == numThreads - 1) ? N : start + rowsPerThread;
        threads.push_back(std::thread(multiplyRange, std::ref(A), std::ref(B), std::ref(C), start, end, N));
        // Aquí se hace el Mapping: cada hilo se "mapea" a una porción de la matriz según el rango [start, end).
        start = end;
    }
    
    // Aglomeration: Se espera a que todos los hilos finalicen su ejecución para combinar (aglutinar) los resultados en la matriz C.
    for (auto &th : threads) {
        th.join();
    }
    
    // Imprimir la matriz resultante C, la cual es el producto de A y B.
    std::cout << "La matriz resultante C es:" << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
