#pragma once
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

class charArray {
private:
	char set[17];
	static int counter;
	int id;

public:
	charArray();
	charArray(unsigned short int A);
	charArray(const charArray& other); // Конструктор копирования
	charArray& operator=(const charArray& other); // Оператор присваивания
	~charArray(); // Деструктор

	unsigned short int CtoS();
	void form(unsigned short int A);
	void print();

	friend charArray operator&(charArray A, charArray B);
	friend charArray operator|(charArray A, charArray B);
	friend charArray operator/(charArray A, charArray B);
};