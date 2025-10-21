#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>

using namespace std;
using ll = long long;

void sieve_segment(ll low, ll high, const vector<ll>& small_primes, vector<ll>& local_primes, ll& local_count) {
    if (low < 2) low = 2;
    if (high <= low) return;

    vector<char> seg(high - low, 1);

    for (ll p : small_primes) {
        if (p * p >= high) break;
        ll start = max(p * p, low);
        ll first = ((start + p - 1) / p) * p;
        for (ll j = first; j < high; j += p) {
            if (j < low) continue;
            seg[j - low] = 0;
        }
    }

    local_count = 0;
    local_primes.clear();
    for (ll i = 0; i < high - low; ++i) {
        if (seg[i]) {
            ll num = low + i;
            local_primes.push_back(num);
            ++local_count;
        }
    }
}

void print_top10(const vector<ll>& top10) {
    cout << "Los 10 mayores numeros primos menores que N: ";
    for (auto it = top10.begin(); it != top10.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
}

int main() {
    cout << "Busqueda de numeros primos menores que N (Multihilo)" << endl;
    ll N;
    cout << "Ingrese N (al menos 10000000): ";
    cin >> N;

    const int num_threads = 10; // Ajusta segun tu CPU (10-20 recomendado)

    auto start = chrono::high_resolution_clock::now();

    // Criba secuencial para primos pequenos hasta sqrt(N)
    ll sqrtN = sqrt(N) + 1;
    vector<char> small_sieve(sqrtN, 1);
    if (sqrtN > 0) small_sieve[0] = 0;
    if (sqrtN > 1) small_sieve[1] = 0;
    for (ll i = 2; i * i < sqrtN; ++i) {
        if (small_sieve[i]) {
            for (ll j = i * i; j < sqrtN; j += i) {
                small_sieve[j] = 0;
            }
        }
    }
    vector<ll> small_primes;
    ll small_count = 0;
    for (ll i = 2; i < sqrtN; ++i) {
        if (small_sieve[i]) {
            small_primes.push_back(i);
            ++small_count;
        }
    }

    // Preparar segmentos para threads (desde sqrtN hasta N)
    ll range_start = sqrtN;
    if (range_start < 2) range_start = 2;
    ll range_size = N - range_start;
    ll block_size = (range_size + num_threads - 1) / num_threads;

    vector<thread> threads;
    vector<vector<ll>> local_primes_list(num_threads);
    vector<ll> local_counts(num_threads, 0);

    for (int t = 0; t < num_threads; ++t) {
        ll low = range_start + t * block_size;
        ll high = min(low + block_size, N);
        threads.emplace_back([t, low, high, &small_primes, &local_primes_list, &local_counts]() {
            sieve_segment(low, high, small_primes, local_primes_list[t], local_counts[t]);
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Calcular conteo total
    ll total_count = small_count;
    for (ll cnt : local_counts) total_count += cnt;

    // Encontrar los 10 mayores (de los segmentos grandes, ya que small son pequenos)
    vector<ll> candidates;
    for (auto& loc : local_primes_list) {
        // Ordenar local descending y tomar top 10 local
        sort(loc.rbegin(), loc.rend());
        for (size_t i = 0; i < min(10ULL, loc.size()); ++i) {
            candidates.push_back(loc[i]);
        }
    }
    // Ordenar candidates descending y tomar top 10
    sort(candidates.rbegin(), candidates.rend());
    vector<ll> top10(candidates.begin(), candidates.begin() + min(10ULL, candidates.size()));

    auto stop = chrono::high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    print_top10(top10);
    cout << "Cantidad de numeros primos menores que N: " << total_count << endl;
    cout << "Tiempo de ejecucion: " << duration.count() << " ms" << endl;
    cout << "Numero de hilos usados: " << num_threads << endl;

    return 0;
}