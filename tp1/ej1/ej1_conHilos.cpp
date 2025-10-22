#include <bits/stdc++.h>
#include <sys/time.h>
#include <thread>
#include <mutex>
using namespace std;

struct Params {
    long double y;
    long double y2;
    long long k_begin;
    long long k_end;
    long double partial;
};

void worker(Params* p) {
    long double pot = p->y * pow(p->y2, (long double)p->k_begin);
    long double acc = 0.0L;

    for (long long k = p->k_begin; k < p->k_end; ++k) {
        long long denom = 2LL * k + 1LL;
        acc += pot / (long double)denom;
        pot *= p->y2;
    }
    p->partial = acc;
}

long double ln_series_parallel(long double x, long long terms, int nthreads) {
    if (x < 1500000) throw runtime_error("x debe ser >= 1500000");
    if (nthreads < 1) nthreads = 1;

    long double y = (x - 1.0L) / (x + 1.0L);
    long double y2 = y * y;

    vector<Params> P(nthreads);
    vector<thread> th(nthreads);

    // Dividimos los datos
    long long base = terms / nthreads;
    long long rem  = terms % nthreads;
    long long cur  = 0;

    for (int i = 0; i < nthreads; ++i) {
        long long take = base + (i < rem ? 1 : 0);
        P[i] = Params{y, y2, cur, cur + take, 0.0L};
        cur += take;
    }

    // Asignamos y ejecutamos cada hilo con los datos que van a trabajar
    for (int i = 0; i < nthreads; ++i)
        th[i] = thread(worker, &P[i]);

    // Verificamos que termine la ejecucion de cada uno
    long double sum = 0.0L;
    for (int i = 0; i < nthreads; ++i) {
        if (th[i].joinable()) th[i].join();
        sum += P[i].partial;
    }
    return 2.0L * sum;
}

int main() {
    long double x;
    long long terms = 10000000LL; // diez millones
    int nthreads;

    cout << "Ingrese x (>=1500000): ";
    if (!(cin >> x)) return 0;
    cout << "Ingrese numero de hilos: ";
    if (!(cin >> nthreads)) return 0;
    timeval t1, t2; 
    gettimeofday(&t1, nullptr);
    long double res = ln_series_parallel(x, terms, nthreads);
    gettimeofday(&t2, nullptr);
    double tiempo = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1e6;

    cout << setprecision(15) << fixed;
    cout << "ln(x) = " << (long double)res << "\n";
    cout << "Tiempo (s) = " << tiempo << "\n";
    return 0;
}
