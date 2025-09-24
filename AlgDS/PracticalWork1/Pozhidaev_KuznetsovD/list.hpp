#pragma once

#include <iostream>
#include <stdexcept>

// Узел списка
struct ListNode {
	int data;
	ListNode* next;
};

// Структура списка
struct LinkedList {
	ListNode* head;  // Первый элемент
	ListNode* tail;  // Последний элемент
	int size;        // Размер списка

	friend LinkedList operator& (LinkedList A, LinkedList B);
	friend LinkedList operator| (LinkedList A, LinkedList B);
	friend LinkedList operator/ (LinkedList A, LinkedList B);

};

LinkedList* create_list();

ListNode* create_node(int value);

void push_back(LinkedList* list, int value);

bool contains(const LinkedList* list, int value);

int get_at(const LinkedList* list, int index);

int get_size(const LinkedList* list);

void print(const LinkedList* list);

LinkedList itol(unsigned short int num);

unsigned short int ltoi(LinkedList A);

LinkedList get(LinkedList* A);
