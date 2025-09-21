#include "bit.hpp"
#include "hex.hpp"
#include "list.hpp"
#include "str.hpp"
bitArray operator& (bitArray A, bitArray B) {
	bitArray C;
	for (int i = 0; i < std::size(A.arr); i++) {
		A.arr[i] && B.arr[i] ? C.arr[i] = 1 : C.arr[i] = 0;
	}
	return C;
}
bitArray operator| (bitArray A, bitArray B) {
	bitArray C;
	for (int i = 0; i < std::size(A.arr); i++) {
		A.arr[i] || B.arr[i] ? C.arr[i] = 1 : C.arr[i] = 0;
	}
	return C;
}
bitArray operator/ (bitArray A, bitArray B) {
	bitArray C;
	for (int i = 0; i < std::size(A.arr); i++) {
		A.arr[i] && !B.arr[i] ? C.arr[i] = 1 : C.arr[i] = 0;
	}
	return C;
}
int main() {
	bitArray A, B, C, D, E;
	A = StoB(65534);//1111 1111 1111 1110
	B = StoB(16898);//0100 0010 0000 0010
	C = StoB(32286);//0111 1110 0001 1110
	D = StoB(10256);//0010 1000 0001 0000
	E = A / B / (C & D);//1001 0101 1110 1100
	print(E);
	return 0;
}