#pragma once
#include <iostream>
#include <iomanip>

const int m = 26;

class Set {
private:
    unsigned int data;
    static int counter;
    int id;

public:
    Set(const char* str = "") : data(0) {
        for (int i = 0; str[i]; ++i) {
            if (str[i] >= 'A' && str[i] <= 'Z') {
                data |= (1u << (str[i] - 'A'));
            }
        }
        id = ++counter;
        std::cout << "[SetWord] Constructor: ID = " << id << ", Data = 0x" << std::hex << std::setw(8) << std::setfill('0') << data << std::dec << "\n";
    }

    Set(const Set& other) : data(other.data) {
        id = ++counter;
        std::cout << "[SetWord] Copy Constructor: ID = " << id << "\n";
    }

    Set& operator=(const Set& other) {
        if (this != &other) {
            data = other.data;
        }
        std::cout << "[SetWord] Assignment: ID = " << id << "\n";
        return *this;
    }

    Set difference(const Set& other) const {
        Set res;
        res.data = data & ~other.data;
        return res;
    }

    Set intersection(const Set& other) const {
        Set res;
        res.data = data & other.data;
        return res;
    }

    Set set_union(const Set& other) const {
        Set res;
        res.data = data | other.data;
        return res;
    }

    void print(const std::string& label = "Set") const {
        std::cout << label << " (ID = " << id << "): ";
        bool empty = true;
        for (int i = 0; i < m; ++i) {
            if (data & (1u << i)) {
                std::cout << static_cast<char>('A' + i);
                empty = false;
            }
        }
        if (empty) std::cout << "{}";
        std::cout << "\n";
    }

    ~Set() {
        std::cout << "[SetWord] Destructor: ID = " << id << "\n";
    }
};

int Set::counter = 0;
