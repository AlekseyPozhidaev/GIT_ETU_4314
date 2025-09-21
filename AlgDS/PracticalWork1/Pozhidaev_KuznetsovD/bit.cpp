/*
Файл со
структурой bit array
функцией вывода для bit array
функцией перевода bit array в short int
функцией перевода short int в bit array

+ перегрузка логических операторов для работы с bit array
*/
#include "bit.hpp"


bool isBitSet(unsigned short int number, int bitPosition) {
	int mask = 1 << bitPosition;
	return ((number & mask) != 0);
}

unsigned short int BtoS(bitArray A) {
	unsigned short int num = 0;
	for (int i = 0; i < std::size(A.arr); i++) {
		if (A.arr[i]) num += pow(2, i);
	}
	return num;
}

bitArray StoB(unsigned short int num) {
	bitArray A;
	for (int i = 0; i < sizeof(num) * 8; i++) {
		A.arr[i] = (isBitSet(num, i));
	}
	return A;
}

void print(bitArray A) {
	for (int i = std::size(A.arr) - 1; i >= 0; i--) {
		std::cout << A.arr[i] << " ";
	}
	std::cout << std::endl;
}

