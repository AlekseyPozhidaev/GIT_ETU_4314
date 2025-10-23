#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>

//#include "SetList.h"
//#include "SetArray.h"
//#include "SetBitArray.h"
#include "SetWord.h"

Set compute_E(const Set& A, const Set& B, const Set& C, const Set& D) {
    Set A_minus_B = A.difference(B);
    Set C_and_D = C.intersection(D);
    Set E = A_minus_B.set_union(C_and_D);
    return E;
}

std::string generate_random_set() {
    std::string result;
    bool used[26] = { false };
    int count = rand() % 27;

    while ((int)result.size() < count) {
        int idx = rand() % 26;
        if (!used[idx]) {
            used[idx] = true;
            result += static_cast<char>('A' + idx);
        }
    }

    return result;
}

std::string input_set(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n'); // Clear buffer
    std::getline(std::cin, input);

    // Filter and remove duplicates
    bool seen[26] = { false };
    std::string filtered;
    for (char ch : input) {
        if (ch >= 'A' && ch <= 'Z' && !seen[ch - 'A']) {
            seen[ch - 'A'] = true;
            filtered += ch;
        }
    }

    return filtered;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    while (true) {
        std::cout << "\n===== MENU =====\n";
        std::cout << "1. Manual input\n";
        std::cout << "2. Random generation\n";
        std::cout << "0. Exit\n";
        std::cout << "Your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // очистка буфера после чтения числа

        if (choice == 0) break;

        std::string A_str, B_str, C_str, D_str;

        if (choice == 1) {
            A_str = input_set("Enter set A (letters A-Z): ");
            B_str = input_set("Enter set B (letters A-Z): ");
            C_str = input_set("Enter set C (letters A-Z): ");
            D_str = input_set("Enter set D (letters A-Z): ");
        } else if (choice == 2) {
            A_str = generate_random_set();
            B_str = generate_random_set();
            C_str = generate_random_set();
            D_str = generate_random_set();

            std::cout << "\nGenerated sets:\n";
            std::cout << "A: " << (A_str.empty() ? "{}" : A_str) << "\n";
            std::cout << "B: " << (B_str.empty() ? "{}" : B_str) << "\n";
            std::cout << "C: " << (C_str.empty() ? "{}" : C_str) << "\n";
            std::cout << "D: " << (D_str.empty() ? "{}" : D_str) << "\n";
        } else {
            std::cout << "Invalid choice!\n";
            continue;
        }

        {
            Set A(A_str.c_str());
            Set B(B_str.c_str());
            Set C(C_str.c_str());
            Set D(D_str.c_str());

            A.print("A");
            B.print("B");
            C.print("C");
            D.print("D");

            Set E = compute_E(A, B, C, D);
            E.print("E");
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        std::cin.ignore();
    }

    std::cout << "\nExiting program.\n";
    return 0;
}
