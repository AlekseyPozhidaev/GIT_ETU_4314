#pragma once
#include <cstddef>
#include <iostream>

class bitArray {
private:
	bool arr[16];
	static int counter;
	int id;

public:
	bitArray();
	bitArray(unsigned short int num);
	bitArray(const bitArray& other);
	bitArray& operator=(const bitArray& other);
	~bitArray();

	bitArray form(unsigned short int num);
	unsigned short int BtoS();
	void print();

	friend bitArray operator& (bitArray A, bitArray B);
	friend bitArray operator| (bitArray A, bitArray B);
	friend bitArray operator/ (bitArray A, bitArray B);
};