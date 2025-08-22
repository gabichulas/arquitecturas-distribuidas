#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

long double ln_series_longdouble(long double x) {
    if (x < 1500000) throw runtime_error("x debe ser > 1500000");
    long long terms = 10000000LL;
    long double y = (x - 1.0L) / (x + 1.0L);         
    long double y2 = y * y;                     
    long double pot = y;                             
    long double acc = 0.0L;

    for (long long n = 0; n < terms; ++n) {
        long long denom = 2LL * n + 1LL;
        acc += pot / (long double)denom;
        pot *= y2;
    }
    return 2.0L * acc;
}

int main() {

    long double x;
    cout << "Ingrese x (>=1500000): ";
    if (!(cin >> x)) return 0;

    timeval t1, t2; 
    gettimeofday(&t1, nullptr);
    long double res = ln_series_longdouble(x);
    gettimeofday(&t2, nullptr);
    double tiempo = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1e6;
    cout << setprecision(15) << fixed;
    cout << "ln(x) = " << (long double)res << "\n";
    cout << "Tiempo (s) = " << tiempo << "\n";
    return 0;
}
