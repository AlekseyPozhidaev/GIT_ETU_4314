#include "HashTable.hpp"
#include "Node.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <unordered_set>

// ====================== GENERATOR ======================
std::vector<int> generate_unique_random(size_t n) {
    std::vector<int> result;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 2000);
    std::unordered_set<int> seen;

    while (result.size() < n) {
        int val = dist(rng);
        if (seen.insert(val).second) {
            result.push_back(val);
        }
    }
    return result;
}

// ====================== COMPUTE EXPRESSION ======================
HashTable compute_expression(const HashTable& A, const HashTable& B,
                             const HashTable& C, const HashTable& D,
                             const HashTable& E) {
    auto a = A.get_unique_keys();
    auto b = B.get_unique_keys();
    auto c = C.get_unique_keys();
    auto d = D.get_unique_keys();
    auto e = E.get_unique_keys();

    std::vector<int> t1, t2, t3, t4;

    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(t1));
    std::set_intersection(c.begin(), c.end(), d.begin(), d.end(), std::back_inserter(t2));
    std::set_difference(t2.begin(), t2.end(), e.begin(), e.end(), std::back_inserter(t3));
    std::set_symmetric_difference(t1.begin(), t1.end(), t3.begin(), t3.end(), std::back_inserter(t4));

    HashTable result;
    for (int x : t4) result.insert(x);
    return result;
}

// ====================== SETS EXPERIMENT ======================
void run_sets_experiment(std::ofstream& log) {
    const int repeats = 30;        // разумное количество повторов для усреднения
    const int max_size = 2000;
    const int step = 10;

    log << "size,time_us,A_size,B_size,C_size,D_size,E_size,result_size\n";

    for (int n = 10; n <= max_size; n += step) {
        double total_time = 0.0;
        int result_size = 0;
        int success = 0;

        for (int rep = 0; rep < repeats; ++rep) {
            try {
                HashTable A, B, C, D, E;

                auto fill = [&](HashTable& t) {
                    auto vals = generate_unique_random(n);
                    for (int v : vals) t.insert(v);
                };

                fill(A); fill(B); fill(C); fill(D); fill(E);

                auto start = std::chrono::high_resolution_clock::now();
                HashTable result = compute_expression(A, B, C, D, E);
                auto end = std::chrono::high_resolution_clock::now();

                total_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                result_size = result.unique_size();
                success++;
            } catch (...) { 
                continue; 
            }
        }

        if (success > 0) {
            double avg = total_time / success;
            log << n << "," << avg << "," << n << "," << n << "," << n << "," << n << "," << n << "," << result_size << "\n";
            
            if (n % 100 == 0) {
                std::cout << "Sets: n = " << n << " → " << avg << " μs\n";
            }
        }
    }
}

// ====================== SEQUENCES EXPERIMENT ======================
void run_sequences_experiment(std::ofstream& log) {
    const int repeats = 30;
    const int max_size = 2000;
    const int step = 10;

    log << "size,erase_time_us,exclude_time_us,mul_time_us,erase_size,exclude_size,mul_size\n";

    for (int n = 10; n <= max_size; n += step) {
        double t_erase = 0, t_exclude = 0, t_mul = 0;
        size_t s_erase = 0, s_exclude = 0, s_mul = 0;
        int success = 0;

        for (int rep = 0; rep < repeats; ++rep) {
            try {
                HashTable seq;
                auto vals = generate_unique_random(n);
                for (int v : vals) seq.insert(v);

                // Erase ~1/3
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    seq.erase_range(0, std::min<size_t>(n/3, seq.size()-1));
                    t_erase += std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::high_resolution_clock::now() - start).count();
                    s_erase = seq.size();
                }

                // Exclude
                {
                    HashTable sub;
                    auto sv = generate_unique_random(std::max(1, n/5));
                    for (int v : sv) sub.insert(v);

                    auto start = std::chrono::high_resolution_clock::now();
                    seq.exclude(sub);
                    t_exclude += std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::high_resolution_clock::now() - start).count();
                    s_exclude = seq.size();
                }

                // Mul x2
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    HashTable res = seq.mul(2);
                    t_mul += std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::high_resolution_clock::now() - start).count();
                    s_mul = res.size();
                }
                success++;
            } catch (...) { 
                continue; 
            }
        }

        if (success > 0) {
            log << n << "," 
                << (t_erase/success) << "," 
                << (t_exclude/success) << "," 
                << (t_mul/success) << ","
                << s_erase << "," << s_exclude << "," << s_mul << "\n";
            
            if (n % 100 == 0) {
                std::cout << "Seq: n = " << n << " done\n";
            }
        }
    }
}

// ====================== MAIN ======================
int main() {
    std::cout << "=== Statistical Experiment (size up to 5000) ===\n\n";

    {
        std::ofstream f("sets_experiment.csv");
        if (f.is_open()) {
            std::cout << "Running Sets experiment...\n";
            run_sets_experiment(f);
            std::cout << "Sets experiment finished → sets_experiment.csv\n\n";
        }
    }

    {
        std::ofstream f("sequences_experiment.csv");
        if (f.is_open()) {
            std::cout << "Running Sequences experiment...\n";
            run_sequences_experiment(f);
            std::cout << "Sequences experiment finished → sequences_experiment.csv\n";
        }
    }

    std::cout << "\nЭксперимент завершён!\n";
    return 0;
}