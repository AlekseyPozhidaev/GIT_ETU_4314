#pragma once
#include <iostream>

class Node {
public:
    char el;
    Node* next;

    static int allocations;
    static int deallocations;

    void* operator new(size_t size) {
        ++allocations;
        std::cout << "[Node] new called\n";
        return ::operator new(size);
    }

    void operator delete(void* ptr) {
        ++deallocations;
        std::cout << "[Node] delete called\n";
        ::operator delete(ptr);
    }

    Node(char c, Node* n = nullptr) : el(c), next(n) {}
};

int Node::allocations = 0;
int Node::deallocations = 0;

class Set {
private:
    Node* head;
    static int counter;
    int id;

public:
    Set(const char* str = "") : head(nullptr) {
        ++counter;
        id = counter;
        bool used[256] = { false };
        Node* tail = nullptr;
        for (int i = 0; str[i]; ++i) {
            unsigned char ch = (unsigned char)str[i];
            if (!used[ch]) {
                used[ch] = true;
                Node* node = new Node(ch);
                if (!head) head = node;
                else tail->next = node;
                tail = node;
            }
        }
        std::cout << "[SetList] Constructor: ID = " << id << "\n";
    }

    Set(const Set& other) : head(nullptr) {
        ++counter;
        id = counter;
        Node* tail = nullptr;
        for (Node* p = other.head; p; p = p->next) {
            Node* node = new Node(p->el);
            if (!head) head = node;
            else tail->next = node;
            tail = node;
        }
        std::cout << "[SetList] Copy Constructor: ID = " << id << "\n";
    }

    Set& operator=(const Set& other) {
        if (this != &other) {
            clear();
            Node* tail = nullptr;
            for (Node* p = other.head; p; p = p->next) {
                Node* node = new Node(p->el);
                if (!head) head = node;
                else tail->next = node;
                tail = node;
            }
        }
        std::cout << "[SetList] Assignment: ID = " << id << "\n";
        return *this;
    }

    Set difference(const Set& B) const {
        bool inB[256] = { false };
        for (Node* p = B.head; p; p = p->next) inB[(unsigned char)p->el] = true;

        Set result;
        Node* tail = nullptr;
        for (Node* p = head; p; p = p->next) {
            if (!inB[(unsigned char)p->el]) {
                Node* node = new Node(p->el);
                if (!result.head) result.head = node;
                else tail->next = node;
                tail = node;
            }
        }
        return result;
    }

    Set intersection(const Set& B) const {
        bool inB[256] = { false };
        for (Node* p = B.head; p; p = p->next) inB[(unsigned char)p->el] = true;

        Set result;
        Node* tail = nullptr;
        for (Node* p = head; p; p = p->next) {
            if (inB[(unsigned char)p->el]) {
                Node* node = new Node(p->el);
                if (!result.head) result.head = node;
                else tail->next = node;
                tail = node;
            }
        }
        return result;
    }

    Set set_union(const Set& B) const {
        bool used[256] = { false };

        Set result;
        Node* tail = nullptr;
        for (Node* p = head; p; p = p->next) {
            if (!used[(unsigned char)p->el]) {
                used[(unsigned char)p->el] = true;
                Node* node = new Node(p->el);
                if (!result.head) result.head = node;
                else tail->next = node;
                tail = node;
            }
        }
        for (Node* p = B.head; p; p = p->next) {
            if (!used[(unsigned char)p->el]) {
                used[(unsigned char)p->el] = true;
                Node* node = new Node(p->el);
                if (!result.head) result.head = node;
                else tail->next = node;
                tail = node;
            }
        }
        return result;
    }

    void print(const std::string& label = "Set") const {
        std::cout << label << " (ID = " << id << "): ";
        for (Node* p = head; p; p = p->next) std::cout << p->el;
        std::cout << "\n";
    }

    ~Set() {
        clear();
        std::cout << "[SetList] Destructor: ID = " << id << "\n";
    }

private:
    void clear() {
        while (head) {
            Node* next = head->next;
            delete head;
            head = next;
        }
    }
};

int Set::counter = 0;
