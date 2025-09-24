struct charArray {
	char set[17];

	friend charArray operator&(charArray A, charArray B);
	friend charArray operator|(charArray A, charArray B);
	friend charArray operator/(charArray A, charArray B);
};

charArray StoC(unsigned short int A);

unsigned short int CtoS(charArray A);

void print(charArray A);
