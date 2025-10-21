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

    map<string, int> patrones;
    vector<string> orden;
    string patron;
    while (getline(patronesFile, patron)) {
        patrones[patron] = 0;
        orden.push_back(patron);
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

    timeval inicio, fin;
    gettimeofday(&inicio, NULL);

    for (auto &p : patrones) {
        int pos = 0;
        while (true) {
            pos = texto.find(p.first, pos);
            if (pos == string::npos) break;
            p.second++;
            pos++; // permitir solapamiento
        }
    }

    gettimeofday(&fin, NULL);

    // Imprimir según el orden original
    for (size_t i = 0; i < orden.size(); ++i) {
        const string &pat = orden[i];
        cout << "el patron " << i << " aparece " << patrones[pat] << " veces\n";
    }

    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;
    cout << "Tiempo de ejecucion: " << tiempo << " segundos" << endl;

    return 0;
}
