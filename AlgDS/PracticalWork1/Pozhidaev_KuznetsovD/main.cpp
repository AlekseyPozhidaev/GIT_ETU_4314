#include "bit.hpp"
#include "list.hpp"
#include "str.hpp"
#include <chrono>

void calc(unsigned short int A, unsigned short int B, unsigned short int C, unsigned short int D) {
	std::cout << "HEX: ";
	std::cout << std::hex << (A & ~B & ~(C & D)) << std::endl;
}
void calc(bitArray A, bitArray B, bitArray C, bitArray D) {
	std::cout << "Bits: ";
	print(A / B / (C & D));
}
void calc(LinkedList A, LinkedList B, LinkedList C, LinkedList D) {
	std::cout << "List: ";
	LinkedList E = (A / B / (C & D));
	print(&E);
}
void calc(charArray A, charArray B, charArray C, charArray D) {
	std::cout << "Chars: ";
	print(A / B / (C & D));
}
void correctionTest() {
	unsigned short int usA, usB, usC, usD, usE;
	bitArray bA, bB, bC, bD, bE;
	LinkedList lA, lB, lC, lD, lE;
	charArray cA, cB, cC, cD, cE;
	for (int l = 0; l < 1000; l++) {
		usA = (unsigned short int)rand(), usB = (unsigned short int)rand(), usC = (unsigned short int)rand(), usD = (unsigned short int)rand();
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
void testB() {
	const int N = 40000;
	bitArray R[N];
	for (int i = 0; i < N; i++) {
		R[i] = StoB((unsigned short int)rand());
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << N / 4 << "  Bits sets time: " << dt << std::endl;
}
void testUS() {
	const int N = 40000;
	unsigned short int R[N];
	for (int i = 0; i < N; i++) {
		R[i] = ((unsigned short int)rand());
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << N / 4 << "   HEX sets time: " << dt << std::endl;
}
void testC() {
	const int N = 40000;
	charArray R[N];
	for (int i = 0; i < N; i++) {
		R[i] = StoC((unsigned short int)rand());
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << N / 4 << " Chars sets time: " << dt << std::endl;
}
void testL() {
	const int N = 40000;
	LinkedList R[N];
	for (int i = 0; i < N; i++) {
		R[i] = itol((unsigned short int)rand());
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N / 4; i++) {
		R[1] / R[i + 1] / (R[i + 2] & R[i + 3]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
	std::cout << "Processing " << N / 4 << " Lists sets time: " << dt << std::endl;
}
int main() {
	//correctionTest();
	std::cout << "All results are in microseconds" << std::endl;
	testB();
	testUS();
	testC();
	testL();
	return 0;
}