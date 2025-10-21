#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

vector<vector<float>> crear_matriz(int tamano, float value) {
    vector<vector<float>> matriz(tamano, vector<float>(tamano));
    for (int i = 0; i < tamano; i++) {
        for (int j = 0; j < tamano; j++) {
            matriz[i][j] = value;
        }
    }
    return matriz;
}

void multiply_rows(int start_row, int end_row, 
                   const vector<vector<float>>& matriz1, 
                   const vector<vector<float>>& matriz2, 
                   vector<vector<float>>& matriz_result, 
                   int tamano) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < tamano; j++) {
            for (int k = 0; k < tamano; k++) {
                matriz_result[i][j] += matriz1[i][k] * matriz2[k][j];
            }
        }
    }
}

float sumatoria_matriz(int tamano, const vector<vector<float>>& matriz) {
    float value = 0.0;
    for (int i = 0; i < tamano; i++) {
        for (int j = 0; j < tamano; j++) {
            value += matriz[i][j];
        }
    }
    return value; 
}

void imprimir_esquinas(const vector<vector<float>>& M) {
    int n = M.size();
    cout << "Esquinas: " << endl;
    cout << "  Superior Izq: " << M[0][0] << endl;
    cout << "  Superior Der: " << M[0][n-1] << endl;
    cout << "  Inferior Izq: " << M[n-1][0] << endl;
    cout << "  Inferior Der: " << M[n-1][n-1] << endl;
}

int main() {
    cout << "Multiplicacion matrices NxN (Multihilo)" << endl;
    cout << "Ingrese el tamanio N: ";
    int n;
    cin >> n;

    float v1, v2;
    cout << "Ingrese el valor para matriz A: ";
    cin >> v1;
    cout << "Ingrese el valor para matriz B: ";
    cin >> v2;

    vector<vector<float>> A = crear_matriz(n, v1);
    vector<vector<float>> B = crear_matriz(n, v2);
    vector<vector<float>> C(n, vector<float>(n, 0.0));  // Inicializar a cero

    const int num_threads = 12;  
    int rows_per_thread = n / num_threads;
    int extra_rows = n % num_threads;

    vector<thread> threads;
    int start_row = 0;

    // Medir tiempo
    auto start = high_resolution_clock::now();

    for (int t = 0; t < num_threads; t++) {
        int end_row = start_row + rows_per_thread + (t < extra_rows ? 1 : 0);
        threads.emplace_back(multiply_rows, start_row, end_row, ref(A), ref(B), ref(C), n);
        start_row = end_row;
    }

    for (auto& th : threads) {
        th.join();
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "\nMatriz A:" << endl;
    imprimir_esquinas(A);
    cout << "Matriz B:" << endl;
    imprimir_esquinas(B);
    cout << "Matriz C (resultado):" << endl;
    imprimir_esquinas(C);

    float suma = sumatoria_matriz(n, C);
    cout << "\nSumatoria de C: " << suma << endl;
    cout << "Tiempo ejecucion: " << duration.count() << " ms" << endl;
    cout << "Numero de hilos usados: " << num_threads << endl;

    return 0;
}