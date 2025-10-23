#pragma once
#include <iostream>
#include <cstring>

class Set {
private:
    char data[27];  // множество в виде строки

public:
    static int counter;
    int id;

    Set(const char* str = "") {
        ++counter;
        id = counter;
        create_array(data, str);
        std::cout << "[SetArray] Constructor: ID = " << id << ", Data = " << data << "\n";
    }

    Set difference(const Set& other) const {
        char res[27];
        array_difference(data, other.data, res);
        return Set(res);
    }

    Set intersection(const Set& other) const {
        char res[27];
        array_intersection(data, other.data, res);
        return Set(res);
    }

    Set set_union(const Set& other) const {
        char res[27];
        array_union(data, other.data, res);
        return Set(res);
    }

    void print(const std::string& label = "Set") const {
        std::cout << label << " (ID = " << id << "): " << (data[0] ? data : "{}") << "\n";
    }

    const char* get_data() const {
        return data;
    }

    ~Set() {
        std::cout << "[SetArray] Destructor: ID = " << id << "\n";
    }

private:
    void create_array(char* arr, const char* str) const {
        int k = 0;
        bool used[256] = { false };
        for (int i = 0; str[i]; ++i) {
            if (!used[(unsigned char)str[i]]) {
                used[(unsigned char)str[i]] = true;
                arr[k++] = str[i];
            }
        }
        arr[k] = 0;
    }

    void array_difference(const char* A, const char* B, char* res) const {
        int k = 0;
        bool inB[256] = { false };
        for (int i = 0; B[i]; ++i) inB[(unsigned char)B[i]] = true;
        for (int i = 0; A[i]; ++i) {
            if (!inB[(unsigned char)A[i]]) res[k++] = A[i];
        }
        res[k] = 0;
    }

    void array_intersection(const char* A, const char* B, char* res) const {
        int k = 0;
        bool inB[256] = { false };
        for (int i = 0; B[i]; ++i) inB[(unsigned char)B[i]] = true;
        for (int i = 0; A[i]; ++i) {
            if (inB[(unsigned char)A[i]]) res[k++] = A[i];
        }
        res[k] = 0;
    }

    void array_union(const char* A, const char* B, char* res) const {
        int k = 0;
        bool used[256] = { false };
        for (int i = 0; A[i]; ++i) {
            if (!used[(unsigned char)A[i]]) {
                used[(unsigned char)A[i]] = true;
                res[k++] = A[i];
            }
        }
        for (int i = 0; B[i]; ++i) {
            if (!used[(unsigned char)B[i]]) {
                used[(unsigned char)B[i]] = true;
                res[k++] = B[i];
            }
        }
        res[k] = 0;
    }
};

int Set::counter = 0;
