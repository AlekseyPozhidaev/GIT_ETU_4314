#include "bit.hpp"

// Конструкторы
bitArray::bitArray() {
	for (int i = 0; i < std::size(arr); i++) {
		arr[i] = false;
	}
}

bitArray::bitArray(unsigned short int num) {
	StoB(num);
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

bitArray bitArray::StoB(unsigned short int num) {
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