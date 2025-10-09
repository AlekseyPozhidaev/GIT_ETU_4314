#pragma once

#include <iostream>

class ListNode {
	public:
		int data;
		ListNode* next;
};

struct LinkedList {
	private:
		ListNode* head;
		ListNode* tail;
		int size;

	public:
		friend LinkedList operator& (LinkedList A, LinkedList B);
		friend LinkedList operator| (LinkedList A, LinkedList B);
		friend LinkedList operator/ (LinkedList A, LinkedList B);
		ListNode* create_node(int value);
		void push_back(int value);

		bool contains(const LinkedList* list, int value);

		int get_at(int index);

		int get_size();

		void print(const LinkedList* list);

		LinkedList itol(unsigned short int num);

		unsigned short int ltoi();

		LinkedList get();

};