#ifndef LABWORK_HPP
#define LABWORK_HPP

#include "HashTable.hpp"
#include <vector>
#include <iostream>

// Генерация n уникальных случайных чисел в диапазоне [0, 2000)
std::vector<int> generate_unique_random(size_t n);

// Вычисление выражения A ∪ B ⊕ (C ∩ D \ E)
// Возвращает хеш-таблицу с результатом (уникальные ключи, порядок соответствует отсортированному результату)
HashTable compute_expression(const HashTable& A, const HashTable& B,
                             const HashTable& C, const HashTable& D,
                             const HashTable& E);

// Демонстрация всех этапов работы
void run_lab();

#endif // LABWORK_HPP