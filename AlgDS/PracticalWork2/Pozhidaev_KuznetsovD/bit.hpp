#pragma once
#include <cstddef> // для std::size
#include <iostream>

class bitArray {
private:
	bool arr[16];

public:
	bitArray StoB(unsigned short int num);
	unsigned short int BtoS();
	void print();

	bitArray();
	bitArray(unsigned short int num);

	friend bitArray operator& (bitArray A, bitArray B);
	friend bitArray operator| (bitArray A, bitArray B);
	friend bitArray operator/ (bitArray A, bitArray B);
};