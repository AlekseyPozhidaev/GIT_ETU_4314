#pragma once
#include <iostream>
#include <new>

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
	static int counter;
	int id;

	static const int POOL_SIZE = 100;
	static void* memory_pool[POOL_SIZE];
	static bool used_blocks[POOL_SIZE];

public:
	static void* operator new(size_t size);
	static void operator delete(void* ptr) noexcept;

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

	LinkedList form(unsigned short int num);
	LinkedList intersection(const LinkedList& other) const;
	LinkedList union_with(const LinkedList& other) const;
	LinkedList difference(const LinkedList& other) const;
	unsigned short int ltoi() const;

	LinkedList get();

	friend LinkedList operator& (const LinkedList& A, const LinkedList& B);
	friend LinkedList operator| (const LinkedList& A, const LinkedList& B);
	friend LinkedList operator/ (const LinkedList& A, const LinkedList& B);
};