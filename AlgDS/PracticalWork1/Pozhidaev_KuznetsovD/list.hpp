#pragma once

#include <iostream>

struct ListNode {
	int data;
	ListNode* next;
};

struct LinkedList {
	ListNode* head;
	ListNode* tail;
	int size;

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
