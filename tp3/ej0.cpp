#include <mpi.h>
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static std::string obtener_ip_salida() {
    // abrir un socket UDP "hacia afuera" y leer la IP local
    // (no envía datos).
    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return "0.0.0.0";

    sockaddr_in dst{};
    dst.sin_family = AF_INET;
    dst.sin_port   = htons(53);
    ::inet_pton(AF_INET, "1.1.1.1", &dst.sin_addr);

    int rc = ::connect(sock, reinterpret_cast<sockaddr*>(&dst), sizeof(dst));
    if (rc < 0) { ::close(sock); return "0.0.0.0"; }

    sockaddr_in local{};
    socklen_t len = sizeof(local);
    rc = ::getsockname(sock, reinterpret_cast<sockaddr*>(&local), &len);
    ::close(sock);
    if (rc < 0) return "0.0.0.0";

    char buf[INET_ADDRSTRLEN] = {0};
    const char* ip = ::inet_ntop(AF_INET, &local.sin_addr, buf, sizeof(buf));
    return ip ? std::string(ip) : "0.0.0.0";
}

int main(int argc, char** argv) {
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        std::cerr << "Error iniciando MPI\n";
        return 1;
    }

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // ID del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Total de procesos

    char name[MPI_MAX_PROCESSOR_NAME];
    int name_len = 0;
    MPI_Get_processor_name(name, &name_len); // Nombre de la máquina

    std::string ip = obtener_ip_salida();

    std::cout << "Hola Mundo! soy el proceso " << rank
              << " de " << size
              << " corriendo en la maquina " << name
              << " IP=" << ip
              << std::endl;

    if (MPI_Finalize() != MPI_SUCCESS) {
        std::cerr << "Error finalizando MPI\n";
        return 1;
    }
    return 0;
}

// mpicxx -O3 -o ej0.out ej0.cpp
// mpirun -n 4 ./ej0.out
// mpirun -n 8 --hostfile machinesfile.txt ./ej0.out