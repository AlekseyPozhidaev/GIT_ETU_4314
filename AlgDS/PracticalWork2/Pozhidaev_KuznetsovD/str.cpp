#include "str.hpp"

unsigned short int charArray::CtoS() {
	unsigned short int B = 0;
	const char U[16] = { "EDCBA9876543210" };
	for (int i = 0; i < 16; i++) {
		if (strchr(this->set, U[i])) B += (1u << 14 - i);
	}
	return B;
}

void charArray::print() {
	const char U[16] = { "EDCBA9876543210" };
	for (int i = 0; this->set[i] != '\0' && std::find(std::begin(U), std::end(U), this->set[i]) != std::end(U); i++) cout << this->set[i];
	std::cout << endl;
}

charArray operator&(charArray A, charArray B) {
	charArray C;
	int pos = 0;
	for (char c : A.set) {
		if (strchr(B.set, c)) C.set[pos++] = c;
	}
	C.set[pos] = '\0';
	return C;
}

charArray operator/(charArray A, charArray B) {
	charArray C;
	int pos = 0;
	for (char c : A.set) {
		if (!strchr(B.set, c)) C.set[pos++] = c;
	}
	C.set[pos] = '\0';
	return C;
}

charArray operator|(charArray A, charArray B) {
	charArray C;
	C = A;
	int pos = strlen(A.set);
	for (char c : B.set) {
		if (!strchr(A.set, c)) C.set[pos++] = c;
	}
	C.set[pos] = '\0';
	return C;
}
