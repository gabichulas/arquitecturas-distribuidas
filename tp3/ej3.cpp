#include <mpi.h>
#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

static string obtener_ip_salida() {
    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return "0.0.0.0";
    sockaddr_in dst{};
    dst.sin_family = AF_INET;
    dst.sin_port   = htons(53);
    ::inet_pton(AF_INET, "1.1.1.1", &dst.sin_addr);
    if (::connect(sock, (sockaddr*)&dst, sizeof(dst)) < 0) { 
        ::close(sock); 
        return "0.0.0.0"; 
    }
    sockaddr_in local{}; 
    socklen_t len = sizeof(local);
    if (::getsockname(sock, (sockaddr*)&local, &len) < 0) { 
        ::close(sock); 
        return "0.0.0.0"; 
    }
    ::close(sock);
    char buf[INET_ADDRSTRLEN] = {0};
    const char* ip = ::inet_ntop(AF_INET, &local.sin_addr, buf, sizeof(buf));
    return ip ? string(ip) : "0.0.0.0";
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = 100000000LL;
    
    if (rank == 0) {
        cout << "=== Producto Escalar de Vectores con MPI ===" << endl;
        cout << "Ingrese el tamaño de los vectores (Enter para usar " << N << "): ";
        long long N_in;
        if (cin >> N_in && N_in > 0) {
            N = N_in;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    MPI_Bcast(&N, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    string my_ip = obtener_ip_salida();
    char host[MPI_MAX_PROCESSOR_NAME]; 
    int host_len = 0;
    MPI_Get_processor_name(host, &host_len);

    long long base = N / size;
    long long rem  = N % size;
    long long begin = rank * base + min<long long>(rank, rem);
    long long take  = base + (rank < rem ? 1 : 0);
    long long end   = begin + take;

    vector<double> local_A(take);
    vector<double> local_B(take);
    
    for (long long i = 0; i < take; ++i) {
        long long global_idx = begin + i;
        local_A[i] = (double)(global_idx + 1);
        local_B[i] = (double)(N - global_idx);
    }

    if (rank == 0) {
        cout << "Tamaño de vectores: " << N << endl;
        cout << "Número de procesos: " << size << endl;
        cout << "Elementos por proceso (aproximado): " << base << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    timeval t1{}, t2{};
    if (rank == 0) gettimeofday(&t1, nullptr);

    double producto_local = 0.0;
    for (long long i = 0; i < take; ++i) {
        producto_local += local_A[i] * local_B[i];
    }

    double producto_global = 0.0;
    MPI_Reduce(&producto_local, &producto_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    const int IP_LEN = 64;
    char my_ip_buf[IP_LEN]; 
    memset(my_ip_buf, 0, sizeof(my_ip_buf));
    snprintf(my_ip_buf, IP_LEN, "%s", my_ip.c_str());
    
    vector<char> all_ips; 
    all_ips.resize(size * IP_LEN, 0);
    MPI_Gather(my_ip_buf, IP_LEN, MPI_CHAR, all_ips.data(), IP_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    vector<long long> all_takes(size);
    MPI_Gather(&take, 1, MPI_LONG_LONG, all_takes.data(), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    vector<double> all_productos(size);
    MPI_Gather(&producto_local, 1, MPI_DOUBLE, all_productos.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        gettimeofday(&t2, nullptr);
        double tiempo = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1e6;

        vector<string> rank_ip(size);
        for (int r = 0; r < size; ++r) {
            rank_ip[r] = string(&all_ips[r * IP_LEN]);
        }

        cout << "\n=== Resultados ===" << endl;
        cout << fixed << setprecision(2);
        
        cout << "\nDistribución de trabajo:" << endl;
        for (int r = 0; r < size; ++r) {
            cout << "Proceso " << r << " (IP: " << rank_ip[r] << ")" << endl;
            cout << "  - Elementos procesados: " << all_takes[r] << endl;
            cout << "  - Producto parcial: " << scientific << all_productos[r] << endl;
        }

        // Resultado final
        cout << "\n=== Producto Escalar Final ===" << endl;
        cout << scientific << setprecision(10);
        cout << "A · B = " << producto_global << endl;
        
        double esperado = (double)N * (N + 1.0) * (N + 2.0) / 6.0;
        cout << "Valor esperado: " << esperado << endl;
        double error_relativo = abs(producto_global - esperado) / esperado * 100.0;
        cout << fixed << setprecision(6);
        cout << "Error relativo: " << error_relativo << "%" << endl;

        cout << "\n=== Tiempo de Ejecución ===" << endl;
        cout << "Tiempo total (MPI): " << tiempo << " segundos" << endl;
        
        double tiempo_estimado_sec = tiempo * size;
        cout << "Speedup estimado: " << (tiempo_estimado_sec / tiempo) << "x" << endl;
    }

    MPI_Finalize();
    return 0;
}

// Compilar: mpicxx -O3 -march=native -o ej3.out ej3.cpp
// Ejecutar local: mpirun -n 4 ./ej3.out
// Ejecutar en cluster: mpirun -n 8 --hostfile machinesfile.txt ./ej3.out
