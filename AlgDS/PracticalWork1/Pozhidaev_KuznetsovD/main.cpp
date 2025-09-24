#include "bit.hpp"
#include "hex.hpp"
#include "list.hpp"
#include "str.hpp"

int main() {
	//bitArray A, B, C, D, E;
	//A = StoB(65534);//1111 1111 1111 1110
	//B = StoB(16898);//0100 0010 0000 0010
	//C = StoB(32286);//0111 1110 0001 1110
	//D = StoB(10256);//0010 1000 0001 0000
	//E = A / B / (C & D);//1001 0101 1110 1100
	LinkedList A, B, C, D, E;
	A = itol(65534);
	B = itol(16898);
	C = itol(32286);
	D = itol(10256);
	E = A / B / (C & D);
	print(&E);
	return 0;
}