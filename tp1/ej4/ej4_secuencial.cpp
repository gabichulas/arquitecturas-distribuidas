#include <bits/stdc++.h>
using namespace std;

// Criba secuencial optimizada: sólo impares. Indexamos el impar 2*i+1 en i.
int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " N\n";
        return 1;
    }
    long long N = atoll(argv[1]);
    if (N <= 2) {
        cout << "Cantidad de primos menores que " << N << ": 0\n";
        cout << "Top 10 mayores primos: (no hay)\n";
        cout << "Tiempo (s): 0\n";
        return 0;
    }

    auto t0 = chrono::high_resolution_clock::now();

    // Cantidad de impares en [1, N): floor((N-1)/2). Index i representa el número 2*i+1.
    const long long max_i = (N - 1) / 2;
    vector<uint8_t> is_composite(max_i + 1, 0); // 0 = primo (candidato), 1 = compuesto
    // Marcamos a partir de 3. El impar con i=1 es 3.
    long long limit = (long long) floor(sqrt((long double)N));
    long long limit_i = (limit - 1) / 2;

    for (long long i = 1; i <= limit_i; ++i) {
        if (!is_composite[i]) {
            long long p = 2*i + 1;
            // Comenzar en p*p. Su índice j0 satisface 2*j0+1 = p*p => j0 = (p*p - 1)/2
            long long start = (p * (long long)p - 1) / 2;
            for (long long j = start; j <= max_i; j += p) {
                is_composite[j] = 1;
            }
        }
    }

    // Recolectar conteo y top 10
    long long count = 1; // contamos el 2 aparte
    vector<long long> top10;
    top10.reserve(10);

    for (long long i = max_i; i >= 1 && (long long)top10.size() < 10; --i) {
        if (!is_composite[i]) {
            top10.push_back(2*i + 1);
        }
    }
    if ((long long)top10.size() < 10 && N > 2) {
        // Si hiciera falta completar (raro salvo N pequeño), revisamos hacia abajo.
        for (long long i = (long long)top10.empty() ? max_i : ((top10.back()-1)/2 - 1);
             i >= 1 && (long long)top10.size() < 10; --i) {
            if (!is_composite[i]) top10.push_back(2*i + 1);
             }
    }

    // Conteo total
    for (long long i = 1; i <= max_i; ++i) {
        if (!is_composite[i]) ++count;
    }

    auto t1 = chrono::high_resolution_clock::now();
    double secs = chrono::duration<double>(t1 - t0).count();

    cout << "Cantidad de primos menores que " << N << ": " << count << "\n";
    cout << "Top 10 mayores primos: ";
    // top10 está de mayor a menor; ya sirve así
    for (size_t k = 0; k < top10.size(); ++k) {
        if (k) cout << ", ";
        cout << top10[k];
    }
    if (top10.empty()) cout << "(no hay)";
    cout << "\n";
    cout << "Tiempo (s): " << fixed << setprecision(6) << secs << "\n";
    return 0;
}
