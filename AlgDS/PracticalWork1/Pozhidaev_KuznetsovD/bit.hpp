#pragma once
#include <iostream>

struct bitArray {
	bool arr[16];
	friend bitArray operator& (bitArray A, bitArray B);
	friend bitArray operator| (bitArray A, bitArray B);
	friend bitArray operator/ (bitArray A, bitArray B);

};

bool isBitSet(unsigned short int number, int bitPosition);

unsigned short int BtoS(bitArray A);

bitArray StoB(unsigned short int num);

void print(bitArray A);

