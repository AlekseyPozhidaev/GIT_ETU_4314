#include "bit.hpp"
#include "list.hpp"
#include "str.hpp"
#include <chrono>

void calc(unsigned short int A, unsigned short int B, unsigned short int C, unsigned short int D) {
	std::cout << "HEX: " << std::endl;
	std::cout << std::hex << A << std::endl;
	std::cout << std::hex << B << std::endl;
	std::cout << std::hex << C << std::endl;
	std::cout << std::hex << D << std::endl;
	std::cout << std::hex << (A & ~B & ~(C & D)) << std::endl;
}
void calc(bitArray A, bitArray B, bitArray C, bitArray D) {
	std::cout << "Bits: " << std::endl;;
	print(A);
	print(B);
	print(C);
	print(D);

	print(A / B / (C & D));
}
void calc(LinkedList A, LinkedList B, LinkedList C, LinkedList D) {
	std::cout << "List: " << std::endl;
	print(&A);
	print(&B);
	print(&C);
	print(&D);
	LinkedList E = (A / B / (C & D));
	print(&E);
}
void calc(charArray A, charArray B, charArray C, charArray D) {
	std::cout << "Chars: " << std::endl;
	print(A);
	print(B);
	print(C);
	print(D);
	print(A / B / (C & D));
}
void correctionTest() {
	unsigned short int usA, usB, usC, usD, usE;
	bitArray bA, bB, bC, bD, bE;
	LinkedList lA, lB, lC, lD, lE;
	charArray cA, cB, cC, cD, cE;
	for (int l = 2; l <= 16; l += 2) {
		std::cout << "Cardinality: " << std::dec << l << std::endl;
		usA = (unsigned short int)(rand() % (1u << l)), usB = (unsigned short int)(rand() % (1u << l))
			, usC = (unsigned short int)(rand() % (1u << l)), usD = (unsigned short int)(rand() % (1u << l));
		bA = StoB(usA), bB = StoB(usB), bC = StoB(usC), bD = StoB(usD);
		lA = itol(usA), lB = itol(usB), lC = itol(usC), lD = itol(usD);
		cA = StoC(usA), cB = StoC(usB), cC = StoC(usC), cD = StoC(usD);
		calc(usA, usB, usC, usD);
		calc(bA, bB, bC, bD);
		calc(lA, lB, lC, lD);
		calc(cA, cB, cC, cD);
		std::cout << std::endl;
	}
}
void testB(int n) {
	const int N = 40000;
	bitArray R[N];
	for (int i = 0; i < N; i++) {
		R[i] = StoB((unsigned short int)(rand() % (int)pow(2, n)));
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << "  Bits sets time: " << dt / (N / 4) << std::endl;
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
void testC(int n) {
	const int N = 40000;
	charArray R[N];
	for (int i = 0; i < N; i++) {
		R[i] = StoC((unsigned short int)(rand() % (int)pow(2, n)));
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << " Chars sets time: " << dt / (N / 4) << std::endl;
}
void testL(int n) {
	const int N = 40000;
	LinkedList R[N];
	for (int i = 0; i < N; i++) {
		R[i] = itol((unsigned short int)(rand() % (int)pow(2, n)));
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << " Lists sets time: " << dt / (N / 4) << std::endl;
}
int main() {
	std::srand(std::time(0));
	std::cout << "Output in format of:\nSet A:\nSet B:\nSet C:\nSet D:\nSet E:\n";
	correctionTest();
	std::cout << "All results are in microseconds" << std::endl;
	for (int i = 2; i <= 16; i += 2) {
		std::cout << "Cardinality: " << std::dec << i << std::endl;
		testB(i);
		testUS(i);
		testC(i);
		testL(i);
		std::cout << std::endl;
	}

	return 0;
}