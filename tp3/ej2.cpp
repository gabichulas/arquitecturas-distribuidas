// patrones_mpi.cpp
#include <mpi.h>
#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

// ---------------------- Helpers ----------------------

// IP de salida (IPv4) sin usar localhost: UDP "connect" + getsockname
static string obtener_ip_salida() {
    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return "0.0.0.0";
    sockaddr_in dst{};
    dst.sin_family = AF_INET;
    dst.sin_port   = htons(53); // DNS
    ::inet_pton(AF_INET, "1.1.1.1", &dst.sin_addr); // Cloudflare (arbitrario)
    if (::connect(sock, (sockaddr*)&dst, sizeof(dst)) < 0) { ::close(sock); return "0.0.0.0"; }
    sockaddr_in local{}; socklen_t len = sizeof(local);
    if (::getsockname(sock, (sockaddr*)&local, &len) < 0) { ::close(sock); return "0.0.0.0"; }
    ::close(sock);
    char buf[INET_ADDRSTRLEN] = {0};
    const char* ip = ::inet_ntop(AF_INET, &local.sin_addr, buf, sizeof(buf));
    return ip ? string(ip) : "0.0.0.0";
}

// Leer archivo completo a string (binario, eficiente)
static bool leer_archivo_completo(const string& path, string& out) {
    ifstream in(path, ios::in | ios::binary);
    if (!in) return false;
    in.seekg(0, ios::end);
    size_t sz = (size_t)in.tellg();
    out.resize(sz);
    in.seekg(0, ios::beg);
    if (sz > 0) in.read(&out[0], sz);
    return true;
}

// Leer patrones (uno por línea), sin recortar espacios internos
static bool leer_patrones_linea(const string& path, vector<string>& pats) {
    ifstream in(path);
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        // remover CR si vino de Windows
        while (!line.empty() && (line.back()=='\r' || line.back()=='\n')) line.pop_back();
        pats.push_back(line);
    }
    return true;
}

// Contar ocurrencias con solapamientos
static int contar_solapado(const string& texto, const string& pat) {
    if (pat.empty()) return 0; // por seguridad (evita bucle infinito)
    int cnt = 0;
    size_t pos = 0;
    while (true) {
        pos = texto.find(pat, pos);
        if (pos == string::npos) break;
        ++cnt;
        ++pos; // permitir solapamiento
    }
    return cnt;
}

// ---------------------- Programa principal ----------------------

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 1) Cargar texto y patrones localmente (el TP pide copiar los archivos en todos los nodos)
    string texto;
    vector<string> patrones;
    bool ok_texto = leer_archivo_completo("texto.txt", texto);
    bool ok_pats  = leer_patrones_linea("patrones.txt", patrones);

    // Verificación básica (si algún rank falla, abortamos todos)
    int ok_local = (ok_texto && ok_pats) ? 1 : 0, ok_global = 0;
    MPI_Allreduce(&ok_local, &ok_global, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    if (!ok_global) {
        if (rank == 0) {
            cerr << "Error: no se pudo leer texto.txt o patrones.txt en todos los procesos\n";
        }
        MPI_Finalize();
        return 1;
    }

    // 2) Balancear patrones entre procesos (bloques casi iguales)
    const int P = (int)patrones.size(); // esperado: 32
    int base = P / size;
    int rem  = P % size;
    int begin = rank * base + min(rank, rem);
    int take  = base + (rank < rem ? 1 : 0);
    int end   = begin + take;

    // 3) Obtener IP propia y nombre de host (para reporte)
    string my_ip = obtener_ip_salida();
    char host[MPI_MAX_PROCESSOR_NAME]; int host_len = 0;
    MPI_Get_processor_name(host, &host_len);

    // 4) Medir tiempo (global): barrera + t1 en rank 0
    MPI_Barrier(MPI_COMM_WORLD);
    timeval t1{}, t2{};
    if (rank == 0) gettimeofday(&t1, nullptr);

    // 5) Cada proceso cuenta SUS patrones (llenamos arrays globales con -1 cuando no es mío)
    vector<int> local_counts(P, -1);
    vector<int> local_owner (P, -1);
    for (int i = begin; i < end; ++i) {
        int c = contar_solapado(texto, patrones[i]);
        local_counts[i] = c;
        local_owner[i]  = rank; // yo fui el "dueño" de este patrón
    }

    // 6) Reducir resultados al root (rank 0)
    //    - Para counts: usamos MPI_MAX sobre [-1 vs conteo>=0]; el válido "gana".
    //    - Para owner:  MPI_MAX sobre [-1 vs rank]; el rank del dueño "gana".
    vector<int> global_counts(P, 0);
    vector<int> global_owner (P, 0);
    MPI_Reduce(local_counts.data(), global_counts.data(), P, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(local_owner .data(), global_owner .data(), P, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // 7) Juntar IPs de cada rank (root necesita mapear owner->IP)
    const int IP_LEN = 64;
    char my_ip_buf[IP_LEN]; memset(my_ip_buf, 0, sizeof(my_ip_buf));
    snprintf(my_ip_buf, IP_LEN, "%s", my_ip.c_str());
    vector<char> all_ips; all_ips.resize(size * IP_LEN, 0);
    MPI_Gather(my_ip_buf, IP_LEN, MPI_CHAR, all_ips.data(), IP_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    // 8) Terminar medición
    if (rank == 0) {
        gettimeofday(&t2, nullptr);
        double tiempo = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1e6;

        // Map rápido rank->IP (desde all_ips)
        vector<string> rank_ip(size);
        for (int r = 0; r < size; ++r) {
            rank_ip[r] = string(&all_ips[r*IP_LEN]);
        }

        // 9) Imprimir resultados con "Buscado por <IP>"
        //    (si un patrón quedó con count==-1 es que nadie lo procesó; no debería pasar)
        for (int i = 0; i < P; ++i) {
            int c   = global_counts[i] < 0 ? 0 : global_counts[i];
            int own = global_owner[i];
            string ip = (own >= 0 && own < size) ? rank_ip[own] : "0.0.0.0";
            cout << "el patron " << i << " aparece " << c << " veces. Buscado por " << ip << "\n";
        }

        cout << fixed << setprecision(6);
        cout << "Tiempo de ejecucion (MPI): " << tiempo << " segundos\n";
    }

    MPI_Finalize();
    return 0;
}
