#include "list.hpp"


LinkedList* create_list() {
	LinkedList* list = new LinkedList;
	list->head = nullptr;
	list->tail = nullptr;
	list->size = 0;
	return list;
}

ListNode* create_node(int value) {
	ListNode* newNode = new ListNode;
	newNode->data = value;
	newNode->next = nullptr;
	return newNode;
}

void push_back(LinkedList* list, int value) {
	ListNode* newNode = create_node(value);

	if (list->tail == nullptr) {
		list->head = list->tail = newNode;
	}
	else {
		list->tail->next = newNode;
		list->tail = newNode;
	}
	list->size++;
}

bool contains(const LinkedList* list, int value) {
	ListNode* current = list->head;
	while (current != nullptr) {
		if (current->data == value) {
			return true;
		}
		current = current->next;
	}
	return false;
}
int get_at(const LinkedList* list, int index) {
	if (index < 0 || index >= list->size) {
		throw std::out_of_range("Индекс вне диапазона!");
	}

	ListNode* current = list->head;
	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->data;
}

int get_size(const LinkedList* list) {
	return list->size;
}

void print(const LinkedList* list) {
	ListNode* current = list->head;
	while (current != nullptr) {
		std::cout << std::hex << current->data << " ";
		current = current->next;
	}
	std::cout << std::endl;
}
LinkedList get(LinkedList* A) {
	LinkedList B;
	B.head = A->head;
	B.tail = A->tail;
	B.size = A->size;
	return B;
}
LinkedList itol(unsigned short int num) {
	LinkedList A = get(create_list());
	for (int i = 0; i < sizeof(num) * 8; i++) {
		if ((num & (1 << i)) != 0) push_back(&A, i);
	}
	return A;
}

unsigned short int ltoi(LinkedList A) {
	unsigned short int num = 0;
	for (int i = 0; i < get_size(&A); i++) {
		num += pow(2, get_at(&A, i));
	}
	return num;
}

LinkedList operator& (LinkedList A, LinkedList B)
{
	int num;
	LinkedList C = get(create_list());
	for (int i = 0; i < get_size(&A); i++) {
		num = get_at(&A, i);
		if (contains(&B, num)) push_back(&C, num);
	}
	return C;
}
LinkedList operator| (LinkedList A, LinkedList B)
{
	int num;
	LinkedList C = get(create_list());
	for (int i = 0; i < get_size(&A); i++) {
		push_back(&C, get_at(&A, i));
	}
	for (int i = 0; i < get_size(&A); i++) {
		num = get_at(&A, i);
		if (!contains(&C, num))push_back(&C, num);
	}
	return C;
}
LinkedList operator/ (LinkedList A, LinkedList B)
{
	int num;
	LinkedList C = get(create_list());
	for (int i = 0; i < get_size(&A); i++) {
		num = get_at(&A, i);
		if (!contains(&B, num)) push_back(&C, num);
	}
	return C;
}
