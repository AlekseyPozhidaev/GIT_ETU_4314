#include "bit.hpp"

int bitArray::counter = 0;

bitArray::bitArray() {
	id = ++counter;
	for (int i = 0; i < std::size(arr); i++) {
		arr[i] = false;
	}
	std::cout << "[bitArray] Constructor (default): ID = " << id << std::endl;
}

bitArray::bitArray(unsigned short int num) {
	id = ++counter;
	form(num);
	std::cout << "[bitArray] Constructor (param): ID = " << id << std::endl;
}

bitArray::bitArray(const bitArray& other) {
	id = ++counter;
	for (int i = 0; i < std::size(arr); i++) {
		arr[i] = other.arr[i];
	}
	std::cout << "[bitArray] Copy Constructor: ID = " << id << " from ID = " << other.id << std::endl;
}

bitArray& bitArray::operator=(const bitArray& other) {
	if (this != &other) {
		for (int i = 0; i < std::size(arr); i++) {
			arr[i] = other.arr[i];
		}
	}
	std::cout << "[bitArray] Assignment: ID = " << id << " from ID = " << other.id << std::endl;
	return *this;
}

bitArray::~bitArray() {
	std::cout << "[bitArray] Destructor: ID = " << id << std::endl;
}

bitArray operator& (bitArray A, bitArray B) {
	bitArray C;
	for (int i = 0; i < std::size(A.arr); i++) {
		A.arr[i] && B.arr[i] ? C.arr[i] = 1 : C.arr[i] = 0;
	}
	return C;
}

bitArray operator| (bitArray A, bitArray B) {
	bitArray C;
	for (int i = 0; i < std::size(A.arr); i++) {
		A.arr[i] || B.arr[i] ? C.arr[i] = 1 : C.arr[i] = 0;
	}
	return C;
}

bitArray operator/ (bitArray A, bitArray B) {
	bitArray C;
	for (int i = 0; i < std::size(A.arr); i++) {
		A.arr[i] && !B.arr[i] ? C.arr[i] = 1 : C.arr[i] = 0;
	}
	return C;
}

unsigned short int bitArray::BtoS() {
	unsigned short int num = 0;
	for (int i = 0; i < std::size(this->arr); i++) {
		if (this->arr[i]) num += (1u << i);
	}
	return num;
}

bitArray bitArray::form(unsigned short int num) {
	for (int i = 0; i < sizeof(num) * 8; i++) {
		this->arr[i] = (num & 1u << i);
	}
	return *this;
}

void bitArray::print() {
	for (int i = std::size(this->arr) - 1; i >= 0; i--) {
		std::cout << this->arr[i] << " ";
	}
	std::cout << std::endl;
}