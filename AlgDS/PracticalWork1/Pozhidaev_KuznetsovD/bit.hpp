#pragma once
#include <cmath>
#include <iostream>

struct bitArray {
	bool arr[16];
};

bool isBitSet(unsigned short int number, int bitPosition);

unsigned short int BtoS(bitArray A);

bitArray StoB(unsigned short int num);

void print(bitArray A);

