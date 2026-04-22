#include "LabWork.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <limits>
#include <iostream>

// --- Helper Functions ---

void print_set(const std::string& title, const std::vector<int>& v) {
    std::cout << "\n--- " << title << " (size: " << v.size() << ") ---\n  ";
    if (v.empty()) {
        std::cout << "[Empty]\n";
        return;
    }
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << std::left << std::setw(6) << v[i];
        if ((i + 1) % 10 == 0 && i + 1 != v.size()) std::cout << "\n  ";
    }
    std::cout << "\n";
}

int get_int_input(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) return val;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error! Please enter an integer.\n";
    }
}

std::vector<int> generate_unique_random(size_t n) {
    std::vector<int> result;
    std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
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

// --- Variant 24 Logic ---

HashTable compute_expression(const HashTable& A, const HashTable& B,
                             const HashTable& C, const HashTable& D,
                             const HashTable& E) {
    auto a_keys = A.get_unique_keys();
    auto b_keys = B.get_unique_keys();
    auto c_keys = C.get_unique_keys();
    auto d_keys = D.get_unique_keys();
    auto e_keys = E.get_unique_keys();

    std::cout << "\n>>> INITIAL SETS <<<\n";
    print_set("Set A", a_keys);
    print_set("Set B", b_keys);
    print_set("Set C", c_keys);
    print_set("Set D", d_keys);
    print_set("Set E", e_keys);

    std::cout << "\n>>> EVALUATING EXPRESSION: A U B ^ (C П D \\ E) <<<\n";

    std::vector<int> T1;
    std::set_union(a_keys.begin(), a_keys.end(), b_keys.begin(), b_keys.end(), std::back_inserter(T1));
    print_set("Step 1: T1 = A U B", T1);

    std::vector<int> T2;
    std::set_intersection(c_keys.begin(), c_keys.end(), d_keys.begin(), d_keys.end(), std::back_inserter(T2));
    print_set("Step 2: T2 = C П D", T2);

    std::vector<int> T3;
    std::set_difference(T2.begin(), T2.end(), e_keys.begin(), e_keys.end(), std::back_inserter(T3));
    print_set("Step 3: T3 = T2 \\ E", T3);

    std::vector<int> T4;
    std::set_symmetric_difference(T1.begin(), T1.end(), T3.begin(), T3.end(), std::back_inserter(T4));
    print_set("Step 4 (RESULT): T4 = T1 ^ T3", T4);

    HashTable result;
    for (int x : T4) result.insert(x);
    return result;
}

void fill_table_interactive(HashTable& table, const std::string& name) {
    std::cout << "\n=== Configuring Set " << name << " ===\n";
    int target_size = get_int_input("Target size: ");
    
    std::cout << "Enter elements (space separated). Type any letter (e.g., 'q') to stop manual input:\n> ";
    
    int count = 0;
    int val;
    while (count < target_size && std::cin >> val) {
        if (!table.contains(val)) {
            table.insert(val);
            count++;
        }
    }
    
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (count < target_size) {
        int remaining = target_size - count;
        std::cout << "Filling " << remaining << " remaining slots with random numbers...\n";
        auto random_vals = generate_unique_random(target_size * 2);
        size_t r_idx = 0;
        while (count < target_size && r_idx < random_vals.size()) {
            if (!table.contains(random_vals[r_idx])) {
                table.insert(random_vals[r_idx]);
                count++;
            }
            r_idx++;
        }
    }
    std::cout << "Set " << name << " created.\n";
}

// --- Sub-Menus ---

void run_sets_mode() {
    std::cout << "\n====================================\n";
    std::cout << "        SETS MODE (Variant 24)      \n";
    std::cout << "====================================\n";
    
    HashTable A, B, C, D, E;
    fill_table_interactive(A, "A");
    fill_table_interactive(B, "B");
    fill_table_interactive(C, "C");
    fill_table_interactive(D, "D");
    fill_table_interactive(E, "E");

    HashTable final_set = compute_expression(A, B, C, D, E);
    std::cout << "\nPress Enter to return to Main Menu...";
    std::cin.get();
}

void run_sequences_mode() {
    std::cout << "\n====================================\n";
    std::cout << "      SEQUENCES MODE (Variant 24)   \n";
    std::cout << "====================================\n";

    HashTable working_sequence;
    bool running = true;
    int v;

    while (running) {
        std::cout << "\n--- SEQUENCE MENU (Size: " << working_sequence.size() << ") ---\n";
        std::cout << "1. Print Sequence\n";
        std::cout << "2. ERASE (Remove range [p1, p2])\n";
        std::cout << "3. EXCL (Exclude sub-sequence)\n";
        std::cout << "4. MUL (Multiply sequence)\n";
        std::cout << "5. Enter new sequence (Manual)\n";
        std::cout << "6. Generate new sequence (Random)\n";
        std::cout << "0. Return to Main Menu\n";
        
        int choice = get_int_input("Action: ");

        switch (choice) {
            case 1:
                std::cout << "Current sequence:\n";
                working_sequence.print();
                break;
            case 2: {
                int p1 = get_int_input("Start index (p1): ");
                int p2 = get_int_input("End index (p2): ");
                try {
                    working_sequence.erase_range(p1, p2);
                    std::cout << "Done.\n";
                    working_sequence.print();
                } catch (const std::exception& e) { 
                    std::cout << "Error: " << e.what() << "\n"; 
                }
                break;
            }
            case 3: {
                std::cout << "Enter sub-sequence to exclude (type 'q' to finish):\n> ";
                HashTable sub;
                while (std::cin >> v) sub.insert(v);
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                working_sequence.exclude(sub);
                std::cout << "Done.\n";
                working_sequence.print();
                break;
            }
            case 4: {
                int n = get_int_input("Multiplier (N): ");
                working_sequence = working_sequence.mul(n);
                std::cout << "Done.\n";
                working_sequence.print();
                break;
            }
            case 5: {
                working_sequence.clear();
                std::cout << "Enter new sequence (type 'q' to finish):\n> ";
                while (std::cin >> v) working_sequence.insert(v);
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            case 6: {
                working_sequence.clear();
                int n = get_int_input("Enter desired sequence size: ");
                std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
                std::uniform_int_distribution<int> dist(0, 100); // Диапазон 0-100 удобен для тестов
                
                for(int i = 0; i < n; ++i) {
                    working_sequence.insert(dist(rng));
                }
                std::cout << "Random sequence generated.\n";
                working_sequence.print();
                break;
            }
            case 0:
                running = false;
                break;
            default:
                std::cout << "Invalid input.\n";
        }
    }
}
// --- Main Application Loop ---

void run_lab() {
    bool running = true;
    while (running) {
        std::cout << "\n====================================\n";
        std::cout << "            MAIN MENU               \n";
        std::cout << "====================================\n";
        std::cout << "1. Sets Mode (Expression evaluation)\n";
        std::cout << "2. Sequences Mode (ERASE, EXCL, MUL)\n";
        std::cout << "0. Exit\n";
        
        int choice = get_int_input("Action: ");

        switch (choice) {
            case 1: run_sets_mode(); break;
            case 2: run_sequences_mode(); break;
            case 0: running = false; break;
            default: std::cout << "Invalid input.\n";
        }
    }
}