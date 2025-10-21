#include <iostream>
#include <stdio.h>
#include <vector>
#include <chrono>

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

template <typename T>
void imprimir_matriz(const vector<vector<T>>& matriz) {
    for (const auto& fila : matriz) {
        for (const auto& elem : fila) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

vector<vector<float>> multiplicar_matrices(
    int tamano, 
    vector<vector<float>> matriz1, 
    vector<vector<float>> matriz2
) {
    vector<vector<float>> matriz(tamano, vector<float>(tamano));
    float value;
    for (int i = 0; i < tamano; i++) {
        for (int j = 0; j < tamano; j++) {
            for (int k = 0; k < tamano; k++){
                matriz[i][j] += matriz1[i][k] * matriz2[k][j];
            }
        }
    }
    return matriz;
}

float sumatoria_matriz(int tamano,vector<vector<float>> matriz) {
    float value = 0.0;
    for (int i = 0; i < tamano; i++) {
        for (int j = 0; j < tamano; j++) {
            value += matriz[i][j];
        }
    }
    return value; 
}

void imprimir_esquinas(const vector<vector<float>> M) {
    int n = M.size();
    cout << "Esquinas: " << endl;
    cout << "  Superior Izq: " << M[0][0] << endl;
    cout << "  Superior Der: " << M[0][n-1] << endl;
    cout << "  Inferior Izq: " << M[n-1][0] << endl;
    cout << "  Inferior Der: " << M[n-1][n-1] << endl;
}

int main() {
    cout << "Multiplicacion matrices NxN" << endl;
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

    // Medir tiempo
    auto start = high_resolution_clock::now();
    vector<vector<float>> C = multiplicar_matrices(n, A, B);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "\nMatriz A:" << endl;
    imprimir_esquinas(A);
    cout << "Matriz B:" << endl;
    imprimir_esquinas(B);
    cout << "Matriz C (resultado):" << endl;
    imprimir_esquinas(C);

    float suma = sumatoria_matriz(n,C);
    cout << "\nSumatoria de C: " << suma << endl;
    cout << "Tiempo ejecucion: " << duration.count() << " ms" << endl;

    return 0;
}