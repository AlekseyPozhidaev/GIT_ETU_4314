#pragma once
#include <iostream>

class bitArray {
	private:
		bool arr[16];

	public:

		bitArray bitArray::StoB(unsigned short int num);
		unsigned short int bitArray::BtoS();
		void bitArray::print();

		bitArray();
		bitArray(unsigned short int num);

		friend bitArray operator& (bitArray A, bitArray B);
		friend bitArray operator| (bitArray A, bitArray B);
		friend bitArray operator/ (bitArray A, bitArray B);
	};

