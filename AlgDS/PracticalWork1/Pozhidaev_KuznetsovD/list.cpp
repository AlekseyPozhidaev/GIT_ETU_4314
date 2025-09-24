/*
Файл со
структурой list
функцией вывода для list
функцией перевода list в short int
функцией перевода short int в list

+ перегрузка логических операторов для работы с list
*/
#include "list.hpp"
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
};

// Создание нового пустого списка
LinkedList* create_list() {
	LinkedList* list = new LinkedList;
	list->head = nullptr;
	list->tail = nullptr;
	list->size = 0;
	return list;
}
// Создание нового узла
ListNode* create_node(int value) {
	ListNode* newNode = new ListNode;
	newNode->data = value;
	newNode->next = nullptr;
	return newNode;
}

// Добавление в конец
void push_back(LinkedList* list, int value) {
	ListNode* newNode = create_node(value);

	if (list->tail == nullptr) {
		// Список пустой
		list->head = list->tail = newNode;
	}
	else {
		list->tail->next = newNode;
		list->tail = newNode;
	}
	list->size++;
}
// Поиск элемента
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

void print_list(const LinkedList* list) {
	ListNode* current = list->head;
	std::cout << "Список: ";
	while (current != nullptr) {
		std::cout << current->data << " ";
		current = current->next;
	}
	std::cout << std::endl;
}
//unsigned short int BtoS(bitArray A) {
//    unsigned short int num = 0;
//    for (int i = 0; i < std::size(A.arr); i++) {
//        if (A.arr[i]) num += pow(2, i);
//    }
//    return num;
//}
//
//bitArray StoB(unsigned short int num) {
//    bitArray A;
//    for (int i = 0; i < sizeof(num) * 8; i++) {
//        A.arr[i] = (isBitSet(num, i));
//    }
//    return A;
//}
//LinkedList itol(unsigned short int A) {
//
//}
//
//unsigned short int ltoi(LinkedList* A) {
//	unsigned short int num = 0;
//	for (int i = 0; i < get_size(A); i++) {
//
//	}
//}

//list operator&(list A, list B){
//
//}
//
//list operator|(list A, list B){
//    
//}
//
//list operator/(list A, list B){
//    
//}