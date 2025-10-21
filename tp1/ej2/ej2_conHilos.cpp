#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <sys/time.h>
using namespace std;

void contar_patron(const string& texto, const string& patron, int& contador) {
    int pos = 0;
    while (true) {
        pos = texto.find(patron, pos);
        if (pos == string::npos) break;
        contador++;
        pos++; // permitir solapamiento
    }
}

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
    getline(textoFile, texto);
    textoFile.close();

    vector<int> contadores(patrones.size(), 0);
    vector<thread> hilos;

    timeval inicio, fin;
    gettimeofday(&inicio, NULL);

    for (int i = 0; i < patrones.size(); i++) {
        hilos.push_back(thread(contar_patron, cref(texto), cref(patrones[i]), ref(contadores[i])));
    }

    for (auto &h : hilos) h.join();

    gettimeofday(&fin, NULL);

    for (int i = 0; i < patrones.size(); i++) {
        cout << "el patron " << i << " aparece " << contadores[i] << " veces" << endl;
    }

    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;
    cout << "Tiempo de ejecucion (multihilo): " << tiempo << " segundos" << endl;

    return 0;
}
