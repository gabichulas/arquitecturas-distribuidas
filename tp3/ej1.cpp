#include <mpi.h>
#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

static long double parcial_ln_segmento(long double y, long double y2,
                                       long long k_begin, long long k_end) {
    if (k_begin >= k_end) return 0.0L;
    // pot = y * (y2)^k_begin
    long double pot = y * powl(y2, (long double)k_begin);
    long double acc = 0.0L;
    for (long long k = k_begin; k < k_end; ++k) {
        long long denom = 2LL * k + 1LL;
        acc += pot / (long double)denom;
        pot *= y2;
    }
    return acc;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // 

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long double x = 1500000.0L; 
    long long terms = 10000000LL;
    if (rank == 0) {
        cerr << "Ingrese x (>=1500000) [Enter para usar 1500000]: ";
        long double x_in;
        if (cin >> x_in) x = x_in;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Difundir parámetros a todos los procesos
    MPI_Bcast(&x, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);   // :contentReference[oaicite:4]{index=4}
    MPI_Bcast(&terms, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD); // :contentReference[oaicite:5]{index=5}

    if (x < 1500000.0L) {
        if (rank == 0) {
            cerr << "x debe ser >= 1500000 (segun el TP)." << endl; // :contentReference[oaicite:6]{index=6}
        }
        MPI_Finalize();
        return 1;
    }

    // Preparar constantes de la serie
    long double y  = (x - 1.0L) / (x + 1.0L);
    long double y2 = y * y;

    // Particionamiento balanceado del rango [0, terms) entre 'size' procesos
    long long base = terms / size;
    long long rem  = terms % size;
    long long k_begin = rank * base + min<long long>(rank, rem);
    long long take    = base + (rank < rem ? 1 : 0);
    long long k_end   = k_begin + take;

    // Sincronizar
    MPI_Barrier(MPI_COMM_WORLD);
    timeval t1{}, t2{};
    if (rank == 0) gettimeofday(&t1, nullptr);

    // Cada rank calcula su parcial
    long double parcial = parcial_ln_segmento(y, y2, k_begin, k_end);

    // Reducir (sumar) los parciales en el root
    long double suma_global = 0.0L;
    MPI_Reduce(&parcial, &suma_global, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        gettimeofday(&t2, nullptr);
        double tiempo = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1e6;

        long double ln_x = 2.0L * suma_global; // serie de Taylor: ln(x) = 2 * sum(...)
        cout << setprecision(15) << fixed;
        cout << "ln(x) = " << ln_x << "\n";
        cout << "Tiempo (s) = " << tiempo << "\n";
        // Nota: Para speedup, corré también con -n 1 y dividí tiempos (S = T1 / Tp). :contentReference[oaicite:8]{index=8}
    }

    MPI_Finalize();  // obligatorio en MPI :contentReference[oaicite:9]{index=9}
    return 0;
}
// mpicxx -O3 -march=native -o ej1.out ej1.cpp 
// mpirun -n 8 ./ej1.out
// mpirun -n 32 --hostfile machinesfile.txt ./ej1.out
