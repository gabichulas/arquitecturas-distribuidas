#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>

using namespace std;
using ll = long long;

void print_top10(const vector<ll>& top10) {
    cout << "Los 10 mayores numeros primos menores que N: ";
    for (auto it = top10.rbegin(); it != top10.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
}

int main() {
    cout << "Busqueda de numeros primos menores que N (Secuencial)" << endl;
    ll N;
    cout << "Ingrese N (al menos 10000000): ";
    cin >> N;

    auto start = chrono::high_resolution_clock::now();

    vector<char> is_prime(N, 1);
    if (N > 0) is_prime[0] = 0;
    if (N > 1) is_prime[1] = 0;

    for (ll i = 2; i * i < N; ++i) {
        if (is_prime[i]) {
            for (ll j = i * i; j < N; j += i) {
                is_prime[j] = 0;
            }
        }
    }

    // Encontrar los 10 mayores primos y el conteo total
    vector<ll> top10;
    ll count = 0;
    for (ll i = 2; i < N; ++i) {
        if (is_prime[i]) ++count;
    }
    for (ll i = N - 1; i >= 2 && top10.size() < 10; --i) {
        if (is_prime[i]) top10.push_back(i);
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    print_top10(top10);
    cout << "Cantidad de numeros primos menores que N: " << count << endl;
    cout << "Tiempo de ejecucion: " << duration.count() << " ms" << endl;

    return 0;
}