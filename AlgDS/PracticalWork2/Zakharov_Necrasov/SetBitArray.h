#pragma once
#include <iostream>
#include <cstring>

const int m = 26; // A-Z

class Set {
private:
    bool data[m];
    static int counter;
    int id;

public:
    Set(const char* str = "") {
        std::memset(data, 0, sizeof(data));
        for (int i = 0; str[i]; ++i) {
            if (str[i] >= 'A' && str[i] <= 'Z') {
                data[str[i] - 'A'] = true;
            }
        }
        id = ++counter;
        std::cout << "[SetBitArray] Constructor: ID = " << id << "\n";
    }

    Set(const Set& other) {
        std::memcpy(data, other.data, sizeof(data));
        id = ++counter;
        std::cout << "[SetBitArray] Copy Constructor: ID = " << id << "\n";
    }

    Set& operator=(const Set& other) {
        if (this != &other) {
            std::memcpy(data, other.data, sizeof(data));
        }
        std::cout << "[SetBitArray] Assignment: ID = " << id << "\n";
        return *this;
    }

    Set difference(const Set& other) const {
        Set res;
        for (int i = 0; i < m; ++i) {
            res.data[i] = data[i] && !other.data[i];
        }
        return res;
    }

    Set intersection(const Set& other) const {
        Set res;
        for (int i = 0; i < m; ++i) {
            res.data[i] = data[i] && other.data[i];
        }
        return res;
    }

    Set set_union(const Set& other) const {
        Set res;
        for (int i = 0; i < m; ++i) {
            res.data[i] = data[i] || other.data[i];
        }
        return res;
    }

    void print(const std::string& label = "Set") const {
        std::cout << label << " (ID = " << id << "): ";
        bool empty = true;
        for (int i = 0; i < m; ++i) {
            if (data[i]) {
                std::cout << static_cast<char>('A' + i);
                empty = false;
            }
        }
        if (empty) std::cout << "{}";
        std::cout << "\n";
    }

    ~Set() {
        std::cout << "[SetBitArray] Destructor: ID = " << id << "\n";
    }
};

int Set::counter = 0;
