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
    dst.sin_port = htons(53);
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

    int N = 1000;
    
    if (rank == 0) {
        cout << "=== Multiplicacion de Matrices con MPI ===" << endl;
        cout << "Ingrese el tamaño NxN (Enter para usar " << N << "): ";
        int N_in;
        if (cin >> N_in && N_in > 0) {
            N = N_in;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    string my_ip = obtener_ip_salida();
    char host[MPI_MAX_PROCESSOR_NAME]; 
    int host_len = 0;
    MPI_Get_processor_name(host, &host_len);

    int base_rows = N / size;
    int extra_rows = N % size;
    int my_rows = base_rows + (rank < extra_rows ? 1 : 0);
    int my_start = rank * base_rows + min(rank, extra_rows);

    vector<double> A_local(my_rows * N);
    vector<double> B(N * N);
    vector<double> C_local(my_rows * N);

    if (rank == 0) {
        vector<double> A(N * N);
        for (int i = 0; i < N * N; ++i) {
            A[i] = (double)(i % 100);
        }
        for (int i = 0; i < N * N; ++i) {
            B[i] = (double)((i * 2) % 100);
        }

        int offset = 0;
        for (int r = 0; r < size; ++r) {
            int rows = base_rows + (r < extra_rows ? 1 : 0);
            if (r == 0) {
                memcpy(A_local.data(), A.data(), my_rows * N * sizeof(double));
            } else {
                MPI_Send(A.data() + offset, rows * N, MPI_DOUBLE, r, 0, MPI_COMM_WORLD);
            }
            offset += rows * N;
        }
    } else {
        MPI_Recv(A_local.data(), my_rows * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Bcast(B.data(), N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Tamaño de matrices: " << N << "x" << N << endl;
        cout << "Número de procesos: " << size << endl;
        cout << "Filas por proceso: " << base_rows << " (+" << extra_rows << " extra)" << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    timeval t1{}, t2{};
    if (rank == 0) gettimeofday(&t1, nullptr);

    for (int i = 0; i < my_rows; ++i) {
        for (int j = 0; j < N; ++j) {
            double sum = 0.0;
            for (int k = 0; k < N; ++k) {
                sum += A_local[i * N + k] * B[k * N + j];
            }
            C_local[i * N + j] = sum;
        }
    }

    vector<double> C;
    if (rank == 0) {
        C.resize(N * N);
        memcpy(C.data(), C_local.data(), my_rows * N * sizeof(double));
        
        int offset = my_rows * N;
        for (int r = 1; r < size; ++r) {
            int rows = base_rows + (r < extra_rows ? 1 : 0);
            MPI_Recv(C.data() + offset, rows * N, MPI_DOUBLE, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += rows * N;
        }
    } else {
        MPI_Send(C_local.data(), my_rows * N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }

    const int IP_LEN = 64;
    char my_ip_buf[IP_LEN]; 
    memset(my_ip_buf, 0, sizeof(my_ip_buf));
    snprintf(my_ip_buf, IP_LEN, "%s", my_ip.c_str());
    
    vector<char> all_ips; 
    all_ips.resize(size * IP_LEN, 0);
    MPI_Gather(my_ip_buf, IP_LEN, MPI_CHAR, all_ips.data(), IP_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    vector<int> all_rows(size);
    MPI_Gather(&my_rows, 1, MPI_INT, all_rows.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        gettimeofday(&t2, nullptr);
        double tiempo = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1e6;

        vector<string> rank_ip(size);
        for (int r = 0; r < size; ++r) {
            rank_ip[r] = string(&all_ips[r * IP_LEN]);
        }

        cout << "\n=== Resultados ===" << endl;
        cout << "\nDistribución de trabajo:" << endl;
        for (int r = 0; r < size; ++r) {
            cout << "Proceso " << r << " (IP: " << rank_ip[r] << ")" << endl;
            cout << "  - Filas procesadas: " << all_rows[r] << endl;
        }

        double suma = 0.0;
        for (int i = 0; i < N * N; ++i) {
            suma += C[i];
        }

        cout << "\n=== Resultado de C = A x B ===" << endl;
        cout << fixed << setprecision(2);
        cout << "Esquina superior izquierda: " << C[0] << endl;
        cout << "Esquina superior derecha: " << C[N-1] << endl;
        cout << "Esquina inferior izquierda: " << C[(N-1)*N] << endl;
        cout << "Esquina inferior derecha: " << C[N*N-1] << endl;
        cout << scientific << setprecision(6);
        cout << "Sumatoria total de C: " << suma << endl;

        cout << "\n=== Tiempo de Ejecución ===" << endl;
        cout << "Tiempo total (MPI): " << tiempo << " segundos" << endl;
    }

    MPI_Finalize();
    return 0;
}

// Compilar: mpicxx -O3 -march=native -o ej4.out ej4.cpp
// Ejecutar local: mpirun -n 4 ./ej4.out
// Ejecutar en cluster: mpirun -n 8 --hostfile machinesfile.txt ./ej4.out