#include "list.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

int LinkedList::counter = 0;
void* LinkedList::memory_pool[POOL_SIZE] = { nullptr };
bool LinkedList::used_blocks[POOL_SIZE] = { false };

void* LinkedList::operator new(size_t size) {
	for (int i = 0; i < POOL_SIZE; ++i) {
		if (!used_blocks[i]) {
			if (!memory_pool[i]) {
				memory_pool[i] = ::operator new(size);
			}
			used_blocks[i] = true;
			return memory_pool[i];
		}
	}
	throw std::bad_alloc();
}

void LinkedList::operator delete(void* ptr) noexcept {
	for (int i = 0; i < POOL_SIZE; ++i) {
		if (memory_pool[i] == ptr) {
			used_blocks[i] = false;
			return;
		}
	}
}

LinkedList::LinkedList() {
	id = ++counter;
	head = nullptr;
	tail = nullptr;
	size = 0;
	std::cout << "[LinkedList] Constructor (default): ID = " << id << std::endl;
}

LinkedList::~LinkedList() {
	std::cout << "[LinkedList] Destructor: ID = " << id << std::endl;
	clear();
}

LinkedList::LinkedList(const LinkedList& other) {
	id = ++counter;
	head = nullptr;
	tail = nullptr;
	size = 0;

	ListNode* current = other.head;
	while (current) {
		push_back(current->data);
		current = current->next;
	}
	std::cout << "[LinkedList] Copy Constructor: ID = " << id << " from ID = " << other.id << std::endl;
}

LinkedList& LinkedList::operator=(const LinkedList& other) {
	if (this != &other) {
		clear();
		ListNode* current = other.head;
		while (current) {
			push_back(current->data);
			current = current->next;
		}
	}
	std::cout << "[LinkedList] Assignment: ID = " << id << " from ID = " << other.id << std::endl;
	return *this;
}

void LinkedList::clear() {
	ListNode* current = head;
	while (current) {
		ListNode* next = current->next;
		delete current;
		current = next;
	}
	head = nullptr;
	tail = nullptr;
	size = 0;
}

ListNode* LinkedList::create_node(int value) {
	ListNode* node = new ListNode();
	node->data = value;
	node->next = nullptr;
	return node;
}

void LinkedList::push_back(int value) {
	ListNode* node = create_node(value);
	if (!head) {
		head = tail = node;
	}
	else {
		tail->next = node;
		tail = node;
	}
	size++;
}

bool LinkedList::contains(int value) const {
	ListNode* current = this->head;
	while (current) {
		if (current->data == value) return true;
		current = current->next;
	}
	return false;
}

int LinkedList::get_at(int index) const {
	if (index < 0 || index >= size) throw std::out_of_range("Индекс вне диапазона");
	ListNode* current = head;
	for (int i = 0; i < index; ++i) current = current->next;
	return current->data;
}

int LinkedList::get_size() const {
	return size;
}

void LinkedList::print() const {
	ListNode* current = this->head;
	std::cout << "[";
	bool first = true;
	while (current) {
		if (!first) {
			std::cout << ", ";
		}
		std::cout << current->data;
		first = false;
		current = current->next;
	}
	std::cout << "]" << std::endl;
}

LinkedList LinkedList::form(unsigned short int num) {
	clear();
	for (int i = 0; i < sizeof(num) * 8; i++) {
		if ((num & (1 << i)) != 0) {
			this->push_back(i);
		}
	}
	return *this;
}

unsigned short int LinkedList::ltoi() const {
	unsigned short int num = 0;
	for (int i = 0; i < this->get_size(); i++) {
		int power = this->get_at(i);
		if (power >= 0 && power < 16) {
			num += (1 << power);
		}
	}
	return num;
}

LinkedList LinkedList::intersection(const LinkedList& other) const {
	LinkedList result;
	ListNode* current = head;
	while (current) {
		if (other.contains(current->data))
			result.push_back(current->data);
		current = current->next;
	}
	return result;
}

LinkedList LinkedList::union_with(const LinkedList& other) const {
	LinkedList result;
	ListNode* current = head;
	while (current) {
		result.push_back(current->data);
		current = current->next;
	}

	current = other.head;
	while (current) {
		if (!result.contains(current->data))
			result.push_back(current->data);
		current = current->next;
	}
	return result;
}

LinkedList LinkedList::difference(const LinkedList& other) const {
	LinkedList result;
	ListNode* current = head;
	while (current) {
		if (!other.contains(current->data))
			result.push_back(current->data);
		current = current->next;
	}
	return result;
}

LinkedList operator&(const LinkedList& A, const LinkedList& B) {
	return A.intersection(B);
}

LinkedList operator|(const LinkedList& A, const LinkedList& B) {
	return A.union_with(B);
}

LinkedList operator/(const LinkedList& A, const LinkedList& B) {
	return A.difference(B);
}