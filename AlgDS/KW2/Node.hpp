#ifndef NODE_HPP
#define NODE_HPP

// Узел для цепочек переполнения хеш-таблицы
struct Node {
    int key;
    Node* next;

    explicit Node(int k) : key(k), next(nullptr) {}
};

#endif // NODE_HPP