#include "str.hpp"

charArray StoC(unsigned short int A) {
	charArray B = {};
	const char U[16] = { "0123456789ABCDE" };
	int pos = 0;
	for (int i = 0; i < 16; i++) {
		if (A & (1u << i)) B.set[pos++] = U[i];
	}
	B.set[pos] = '\0';
	return B;
}

unsigned short int CtoS(charArray A) {
	unsigned short int B = 0;
	const char U[16] = { "EDCBA9876543210" };
	for (int i = 0; i < 16; i++) {
		if (strchr(A.set, U[i])) B += (1u << 14 - i);
	}
	return B;
}

void print(charArray A) {
	const char U[16] = { "EDCBA9876543210" };
	for (int i = 0; A.set[i] != '\0' && std::find(std::begin(U), std::end(U), A.set[i]) != std::end(U); i++) cout << A.set[i];
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
