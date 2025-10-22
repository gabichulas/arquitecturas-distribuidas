#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sys/time.h>
using namespace std;

int main() {
    ifstream patronesFile("patrones.txt");
    if (!patronesFile) {
        cout << "No se pudo abrir patrones.txt" << endl;
        return 1;
    }

    vector<string> patrones;
    string patron;
    while (getline(patronesFile, patron)) {
        patrones.push_back(patron);
    }
    patronesFile.close();

    ifstream textoFile("texto.txt");
    if (!textoFile) {
        cout << "No se pudo abrir texto.txt" << endl;
        return 1;
    }

    string texto;
    getline(textoFile, texto); // el archivo tiene una sola línea
    textoFile.close();

    vector<int> contadores(patrones.size(), 0); // inicializamos contadores

    timeval inicio, fin;
    gettimeofday(&inicio, NULL);

    // Buscamos cada clave del diccionario en el texto
    for (int i = 0; i < patrones.size(); i++) {
        int pos = 0;
        while (true) {
            pos = texto.find(patrones[i], pos);
            if (pos == string::npos) break;
            contadores[i]++;  // Incrementa el contador para el patrón i
            pos++;  // Permitir solapamiento
        }
    }

    gettimeofday(&fin, NULL);

    // Mostrar resultados
    for (int i = 0; i < patrones.size(); i++) {
        cout << "El patrón '" << i << "' aparece " << contadores[i] << " veces" << endl;
    }

    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;
    cout << "Tiempo de ejecucion: " << tiempo << " segundos" << endl;

    return 0;
}
