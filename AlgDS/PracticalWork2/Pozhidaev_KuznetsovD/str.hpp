#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

class charArray {
private:
	char set[17];

public:
	charArray();
	charArray(unsigned short int A) {
		const char U[16] = { "0123456789ABCDE" };
		int pos = 0;
		for (int i = 0; i < 16; i++) {
			if (A & (1u << i)) this->set[pos++] = U[i];
		}
		this->set[pos] = '\0';
	}

	unsigned short int CtoS();
	void StoC(unsigned short int A);
	void print();

	friend charArray operator&(charArray A, charArray B);
	friend charArray operator|(charArray A, charArray B);
	friend charArray operator/(charArray A, charArray B);
};