#include "bit.hpp"
#include "list.hpp"
#include "str.hpp"
#include <chrono>
#include <cmath>

void gcInput(unsigned short int& intValue) {
	string s;
	unsigned short int o;
	std::cin.clear();
	std::cin.ignore();
	cout << "Please enter an Element: ";
	char d[100];
	std::cin.getline(d, sizeof(d));
	while (*d && strspn(d, "0123456789abcdefABCDEF") != strlen(d)) {
		cout << "Invalid input. Please enter an Element: ";
		std::cin.getline(d, sizeof(d));
	}
	if (d[0] > 57) {
		intValue = d[0] - 87;
	}
	else {
		intValue = std::stoi(d);
	}
}

void InputHex(unsigned short int& num) {
	num = 0;
	unsigned short int temp;
	string ch;

	while (ch != "0") {
		std::cout << "Do you want to continue or stop to enter a set\nQuit - 0\nContinue - anything else" << std::endl;
		std::cin >> ch;
		if (ch != "0") {
			gcInput(temp);
			if ((num & (1u << temp)) == 0) num += (1u << temp);
		}
	}
}
template <typename T>
void calc(T& A, T& B, T& C, T& D) {
	A.print();
	B.print();
	C.print();
	D.print();
	(A / B / (C & D)).print();
}
void calc(unsigned short int A, unsigned short int B, unsigned short int C, unsigned short int D) {
	std::cout << "HEX: " << std::endl;
	std::cout << std::hex << A << std::endl;
	std::cout << std::hex << B << std::endl;
	std::cout << std::hex << C << std::endl;
	std::cout << std::hex << D << std::endl;
	std::cout << std::hex << (A & ~B & ~(C & D)) << std::endl;
}
void correctionTest() {
	unsigned short int usA, usB, usC, usD;
	bitArray bA, bB, bC, bD;
	LinkedList lA, lB, lC, lD;
	charArray cA, cB, cC, cD;
	for (int l = 2; l <= 16; l += 2) {
		std::cout << "Cardinality: " << std::dec << l << std::endl;
		usA = (unsigned short int)(rand() % (1u << l)), usB = (unsigned short int)(rand() % (1u << l))
			, usC = (unsigned short int)(rand() % (1u << l)), usD = (unsigned short int)(rand() % (1u << l));
		bA.form(usA), bB.form(usB), bC.form(usC), bD.form(usD);
		lA.form(usA), lB.form(usB), lC.form(usC), lD.form(usD);
		cA.form(usA), cB.form(usB), cC.form(usC), cD.form(usD);
		calc(usA, usB, usC, usD);
		calc<bitArray>(bA, bB, bC, bD);
		calc<LinkedList>(lA, lB, lC, lD);
		calc<charArray>(cA, cB, cC, cD);
		std::cout << std::endl;
	}
}
template <typename T>
void test(int n) {
	const int N = 40000;
	T R[N];
	for (int i = 0; i < N; i++) {
		R[i].form((unsigned short int)(rand() % (int)pow(2, n)));
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << typeid(T).name() << "  sets time: " << dt / (N / 4) << std::endl;
}
void testUS(int n) {
	const int N = 40000;
	unsigned short int R[N];
	for (int i = 0; i < N; i++) {
		R[i] = ((unsigned short int)(rand() % (int)pow(2, n)));
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << "   HEX sets time: " << dt / (N / 4) << std::endl;
}
int main() {
	std::srand(std::time(0));
	std::cout << "Output in format of:\nSet A:\nSet B:\nSet C:\nSet D:\n";
	string ch;
	cout << "choose the option\n Manual input - 0\nRandom tests - Anything else\n";
	std::cin >> ch;
	if (ch == "0") {
		unsigned short int usA, usB, usC, usD;
		bitArray bA, bB, bC, bD;
		LinkedList lA, lB, lC, lD;
		charArray cA, cB, cC, cD;
		cout << "Please enter a set A:" << endl;
		InputHex(usA);
		cout << "Please enter a set B:" << endl;
		InputHex(usB);
		cout << "Please enter a set C:" << endl;
		InputHex(usC);
		cout << "Please enter a set D:" << endl;
		InputHex(usD);
		bA.form(usA), bB.form(usB), bC.form(usC), bD.form(usD);
		lA.form(usA), lB.form(usB), lC.form(usC), lD.form(usD);
		cA.form(usA), cB.form(usB), cC.form(usC), cD.form(usD);
		calc(usA, usB, usC, usD);
		calc<bitArray>(bA, bB, bC, bD);
		calc<LinkedList>(lA, lB, lC, lD);
		calc<charArray>(cA, cB, cC, cD);
	}
	else {
		correctionTest();
		//std::cout << "All results are in microseconds" << std::endl;
		//for (int i = 2; i <= 16; i += 2) {
		//	std::cout << "Cardinality: " << std::dec << i << std::endl;
		//	test<bitArray>(i);
		//	testUS(i);
		//	test<charArray>(i);
		//	test<LinkedList>(i);
		//	std::cout << std::endl;
		//}
	}
	return 0;
}