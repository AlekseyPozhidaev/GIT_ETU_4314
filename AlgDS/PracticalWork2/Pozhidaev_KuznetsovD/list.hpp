#pragma once

#include <iostream>
#include <new>  // для std::bad_alloc

class ListNode {
public:
	int data;
	ListNode* next;
};

class LinkedList {
private:
	ListNode* head;
	ListNode* tail;
	int size;

	// Статический массив "пула" для хранения объектов
	static const int POOL_SIZE = 100;
	static void* memory_pool[POOL_SIZE];
	static bool used_blocks[POOL_SIZE];

public:
	// Перегрузка операторов new и delete
	static void* operator new(size_t size);
	static void operator delete(void* ptr) noexcept;

	// Методы
	LinkedList();
	~LinkedList();

	LinkedList(const LinkedList& other);

	LinkedList& operator=(const LinkedList& other);

	void clear();

	ListNode* create_node(int value);
	void push_back(int value);
	bool contains(int value) const;
	int get_at(int index) const;
	int get_size() const;
	void print() const;

	LinkedList itol(unsigned short int num);
	LinkedList intersection(const LinkedList& other) const;
	LinkedList union_with(const LinkedList& other) const;
	LinkedList difference(const LinkedList& other) const;
	unsigned short int ltoi() const;

	LinkedList get();

	// Перегрузки операторов для объединения / пересечения / разности
	friend LinkedList operator& (const LinkedList& A, const LinkedList& B);
	friend LinkedList operator| (const LinkedList& A, const LinkedList& B);
	friend LinkedList operator/ (const LinkedList& A, const LinkedList& B);
};
