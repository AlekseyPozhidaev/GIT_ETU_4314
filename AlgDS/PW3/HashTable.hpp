#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_set>

#include "Node.hpp"

// Хеш-таблица с цепочками переполнения и поддержкой последовательности (порядок вставки)
class HashTable {
public:
    using iterator = typename std::vector<Node*>::iterator;
    using const_iterator = typename std::vector<Node*>::const_iterator;

    // Конструктор: начальный размер таблицы (простое число)
    explicit HashTable(size_t initial_size = 101);
    // Копирующий конструктор (глубокое копирование)
    HashTable(const HashTable& other);
    // Оператор присваивания
    HashTable& operator=(const HashTable& other);
    // Деструктор
    ~HashTable();

    // Вставка ключа (допускаются дубликаты)
    void insert(int key);
    // Удаление одного экземпляра ключа (первого найденного)
    bool remove_one(int key);
    // Удаление всех экземпляров ключа
    size_t remove_all(int key);
    // Количество экземпляров ключа
    size_t count(int key) const;
    // Проверка наличия хотя бы одного экземпляра
    bool contains(int key) const;
    // Очистка всей таблицы
    void clear();

    // --- Операции над последовательностью (вариант 24) ---
    // Удаление элементов в диапазоне позиций [p1, p2] (индексы в порядке вставки)
    void erase_range(size_t p1, size_t p2);
    // Исключение подпоследовательности (первое вхождение)
    // other – последовательность, которая должна быть исключена
    void exclude(const HashTable& other);
    // Размножение: сцепление последовательности самой с собой n раз
    HashTable mul(size_t n) const;

    // --- Получение уникальных ключей (для операций над множествами) ---
    std::vector<int> get_unique_keys() const;

    // --- Итераторы для совместимости с STL (обход в порядке вставки) ---
    iterator begin() { return order_.begin(); }
    iterator end()   { return order_.end(); }
    const_iterator begin() const { return order_.begin(); }
    const_iterator end()   const { return order_.end(); }

    // --- Вспомогательные методы ---
    size_t size() const { return order_.size(); }          // общее количество элементов (с дубликатами)
    size_t unique_size() const { return get_unique_keys().size(); }
    void print() const;                                    // печать последовательности (порядок вставки)

private:
    std::vector<Node*> buckets_;   // экстенты (списки цепочек)
    size_t m_;                      // размер таблицы
    std::vector<Node*> order_;      // указатели на узлы в порядке вставки (для последовательности)

    // Хеш-функция (простой остаток от деления)
    size_t hash(int key) const { return static_cast<size_t>(key) % m_; }

    // Внутренние вспомогательные методы
    void rehash(size_t new_size);
    void copy_from(const HashTable& other);
    void free_nodes();
};

// ---------- Реализация ----------

inline HashTable::HashTable(size_t initial_size)
    : m_(initial_size), buckets_(initial_size, nullptr) {}

inline HashTable::HashTable(const HashTable& other) {
    copy_from(other);
}

inline HashTable& HashTable::operator=(const HashTable& other) {
    if (this != &other) {
        free_nodes();
        copy_from(other);
    }
    return *this;
}

inline HashTable::~HashTable() {
    free_nodes();
}

inline void HashTable::free_nodes() {
    for (Node* head : buckets_) {
        while (head) {
            Node* next = head->next;
            delete head;
            head = next;
        }
    }
    buckets_.clear();
    order_.clear();
}

inline void HashTable::copy_from(const HashTable& other) {
    m_ = other.m_;
    buckets_.resize(m_, nullptr);
    // Глубокое копирование узлов с сохранением цепочек и порядка
    std::unordered_map<Node*, Node*> old_to_new;
    for (Node* node : other.order_) {
        Node* new_node = new Node(node->key);
        order_.push_back(new_node);
        old_to_new[node] = new_node;
    }
    for (size_t i = 0; i < m_; ++i) {
        Node* cur = other.buckets_[i];
        Node* prev_new = nullptr;
        while (cur) {
            Node* new_node = old_to_new[cur];
            if (!prev_new)
                buckets_[i] = new_node;
            else
                prev_new->next = new_node;
            prev_new = new_node;
            cur = cur->next;
        }
    }
}

inline void HashTable::insert(int key) {
    // Строгая гарантия: сначала создаём узел
    Node* new_node = new Node(key);
    size_t idx = hash(key);
    try {
        order_.push_back(new_node);
        // Вставка в начало цепочки (проще)
        new_node->next = buckets_[idx];
        buckets_[idx] = new_node;
    } catch (...) {
        delete new_node;
        throw;
    }
}

inline bool HashTable::remove_one(int key) {
    size_t idx = hash(key);
    Node* prev = nullptr;
    Node* cur = buckets_[idx];
    while (cur) {
        if (cur->key == key) {
            // Удаляем из цепочки
            if (prev)
                prev->next = cur->next;
            else
                buckets_[idx] = cur->next;
            // Удаляем из order_ (первое вхождение)
            auto it = std::find(order_.begin(), order_.end(), cur);
            if (it != order_.end())
                order_.erase(it);
            delete cur;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

inline size_t HashTable::remove_all(int key) {
    size_t removed = 0;
    while (remove_one(key))
        ++removed;
    return removed;
}

inline size_t HashTable::count(int key) const {
    size_t cnt = 0;
    size_t idx = hash(key);
    Node* cur = buckets_[idx];
    while (cur) {
        if (cur->key == key)
            ++cnt;
        cur = cur->next;
    }
    return cnt;
}

inline bool HashTable::contains(int key) const {
    return count(key) > 0;
}

inline void HashTable::clear() {
    free_nodes();
    buckets_.assign(m_, nullptr);
    order_.clear();
}

inline void HashTable::erase_range(size_t p1, size_t p2) {
    if (p1 > p2 || p2 >= order_.size())
        throw std::out_of_range("erase_range: invalid indices");

    // Сначала удаляем узлы из цепочек хеш-таблицы
    for (size_t i = p1; i <= p2; ++i) {
        Node* node = order_[i];
        size_t idx = hash(node->key);
        
        Node* prev = nullptr;
        Node* cur = buckets_[idx];
        
        while (cur && cur != node) {
            prev = cur;
            cur = cur->next;
        }
        
        if (cur) {
            if (prev) prev->next = cur->next;
            else buckets_[idx] = cur->next;
            delete cur; // Удаляем сам узел
        }
    }

    // И только потом удаляем "битые" указатели из вектора order_ одним махом
    order_.erase(order_.begin() + p1, order_.begin() + p2 + 1);
}

// Исключение подпоследовательности (первое вхождение)
inline void HashTable::exclude(const HashTable& other) {
    if (other.order_.empty())
        return;
    // Ищем первое вхождение последовательности other в this->order_
    auto it = std::search(order_.begin(), order_.end(),
                          other.order_.begin(), other.order_.end(),
                          [](Node* a, Node* b) { return a->key == b->key; });
    if (it == order_.end())
        return; // не найдено
    size_t start = std::distance(order_.begin(), it);
    size_t len = other.order_.size();
    erase_range(start, start + len - 1);
}

// Размножение: сцепление последовательности самой с собой n раз
inline HashTable HashTable::mul(size_t n) const {
    HashTable result(m_);
    if (n == 0 || order_.empty())
        return result;
    for (size_t rep = 0; rep < n; ++rep) {
        for (Node* node : order_) {
            result.insert(node->key);
        }
    }
    return result;
}

// Получить отсортированный вектор уникальных ключей (для операций над множествами)
inline std::vector<int> HashTable::get_unique_keys() const {
    std::unordered_set<int> unique_set;
    for (Node* node : order_) {
        unique_set.insert(node->key);
    }
    std::vector<int> keys(unique_set.begin(), unique_set.end());
    std::sort(keys.begin(), keys.end());
    return keys;
}

inline void HashTable::print() const {
    std::cout << "[\n  ";
    for (size_t i = 0; i < order_.size(); ++i) {
        std::cout << order_[i]->key << "\t";
        if ((i + 1) % 10 == 0) std::cout << "\n  "; // Перенос каждые 10 элементов
    }
    std::cout << "\n] (size=" << order_.size() << ")\n";
}

#endif // HASHTABLE_HPP