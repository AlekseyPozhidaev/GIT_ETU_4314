#include "str.hpp"

// Конструктор по умолчанию
charArray::charArray() {
	set[0] = '\0';
}

unsigned short int charArray::CtoS() {
	unsigned short int B = 0;
	const char U[16] = { "EDCBA9876543210" };
	for (int i = 0; i < 16; i++) {
		if (strchr(this->set, U[i])) B += (1u << 14 - i);
	}
	return B;
}

void charArray::StoC(unsigned short int A) {
	const char U[16] = { "0123456789ABCDE" };
	int pos = 0;
	for (int i = 0; i < 16; i++) {
		if (A & (1u << i)) this->set[pos++] = U[i];
	}
	this->set[pos] = '\0';
}

void charArray::print() {
	const char U[16] = { "EDCBA9876543210" };
	for (int i = 0; this->set[i] != '\0' && std::find(std::begin(U), std::end(U), this->set[i]) != std::end(U); i++) cout << this->set[i];
	std::cout << endl;
}

charArray operator&(charArray A, charArray B) {
	charArray C;
	int pos = 0;
	for (int i = 0; A.set[i] != '\0'; i++) {
		char c = A.set[i];
		if (strchr(B.set, c)) C.set[pos++] = c;
	}
	C.set[pos] = '\0';
	return C;
}

charArray operator/(charArray A, charArray B) {
	charArray C;
	int pos = 0;
	for (int i = 0; A.set[i] != '\0'; i++) {
		char c = A.set[i];
		if (!strchr(B.set, c)) C.set[pos++] = c;
	}
	C.set[pos] = '\0';
	return C;
}

charArray operator|(charArray A, charArray B) {
	charArray C;
	C = A;
	int pos = strlen(A.set);
	for (int i = 0; B.set[i] != '\0'; i++) {
		char c = B.set[i];
		if (!strchr(A.set, c)) C.set[pos++] = c;
	}
	C.set[pos] = '\0';
	return C;
}