#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <locale>
#include <string>
#include <windows.h>
using namespace std;

void showSystemInfo() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	cout << "Информация о вычислительной системе:" << endl;
	cout << "Количество процессоров: " << si.dwNumberOfProcessors << endl;
	cout << "Размер страницы: " << si.dwPageSize << " байт" << endl;
	cout << "Минимальный адрес пользовательского пространства: 0x" << hex << (unsigned long long)si.lpMinimumApplicationAddress << endl;
	cout << "Максимальный адрес пользовательского пространства: 0x" << hex << (unsigned long long)si.lpMaximumApplicationAddress << dec << endl;
	cout << "Тип процессора: " << si.dwProcessorType << endl;
	cout << "Уровень процессора: " << si.wProcessorLevel << endl;
}

void showMemoryStatus() {
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	cout << "Статус виртуальной памяти:" << endl;
	cout << "Всего физической памяти: " << ms.dwTotalPhys / 1024 / 1024 << " МБ" << endl;
	cout << "Доступно физической памяти: " << ms.dwAvailPhys / 1024 / 1024 << " МБ" << endl;
	cout << "Всего виртуальной памяти: " << ms.dwTotalVirtual / 1024 / 1024 << " МБ" << endl;
	cout << "Доступно виртуальной памяти: " << ms.dwAvailVirtual / 1024 / 1024 << " МБ" << endl;
	cout << "Всего страниц подкачки: " << ms.dwTotalPageFile / 1024 / 1024 << " МБ" << endl;
	cout << "Доступно страниц подкачки: " << ms.dwAvailPageFile / 1024 / 1024 << " МБ" << endl;
}

void queryMemoryStatus() {
	cout << "Введите адрес участка памяти (в hex): 0x";
	unsigned long long addrInput;
	cin >> hex >> addrInput;
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T res = VirtualQuery((LPCVOID)addrInput, &mbi, sizeof(mbi));
	if (res == 0) {
		cout << "VirtualQuery вернула ошибку: " << GetLastError() << endl;
		return;
	}
	cout << "Состояние участка памяти по адресу 0x" << hex << addrInput << ":" << endl;
	cout << "Базовый адрес региона: 0x" << hex << (unsigned long long)mbi.BaseAddress << endl;
	cout << "Размер региона: " << dec << mbi.RegionSize << " байт" << endl;
	cout << "Состояние (State): ";
	if (mbi.State == MEM_COMMIT) cout << "MEM_COMMIT (зафиксировано)";
	else if (mbi.State == MEM_RESERVE) cout << "MEM_RESERVE (зарезервировано)";
	else if (mbi.State == MEM_FREE) cout << "MEM_FREE (свободно)";
	else cout << mbi.State;
	cout << endl;
	cout << "Защита (Protect): 0x" << hex << mbi.Protect << endl;
	cout << dec;
	cout << "Тип (Type): ";
	if (mbi.Type == MEM_PRIVATE) cout << "MEM_PRIVATE";
	else if (mbi.Type == MEM_MAPPED) cout << "MEM_MAPPED";
	else if (mbi.Type == MEM_IMAGE) cout << "MEM_IMAGE";
	else cout << mbi.Type;
	cout << endl;
}

void separateReserveCommitAuto() {
	const SIZE_T regionSize = 4096;
	cout << "Раздельное резервирование региона и передача физической памяти (автоматический режим)" << endl;
	LPVOID reservedAddr = VirtualAlloc(NULL, regionSize, MEM_RESERVE, PAGE_READWRITE);
	if (reservedAddr == NULL) {
		cout << "Ошибка VirtualAlloc (MEM_RESERVE): " << GetLastError() << endl;
		return;
	}
	cout << "Регион успешно зарезервирован по адресу: 0x" << hex << (unsigned long long)reservedAddr << endl;
	cout << dec;
	LPVOID committedAddr = VirtualAlloc(reservedAddr, regionSize, MEM_COMMIT, PAGE_READWRITE);
	if (committedAddr == NULL) {
		cout << "Ошибка VirtualAlloc (MEM_COMMIT): " << GetLastError() << endl;
		VirtualFree(reservedAddr, 0, MEM_RELEASE);
		return;
	}
	cout << "Физическая память успешно передана (коммит выполнен)." << endl;
	cout << "Освободить регион сейчас? (y/n): ";
	char choice;
	cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		if (VirtualFree(reservedAddr, 0, MEM_RELEASE)) {
			cout << "Регион успешно освобожден (VirtualFree)." << endl;
		}
		else {
			cout << "Ошибка VirtualFree: " << GetLastError() << endl;
		}
	}
	else {
		cout << "Регион оставлен выделенным. Адрес: 0x" << hex << (unsigned long long)reservedAddr << endl;
		cout << dec;
	}
}

void separateReserveCommitManual() {
	const SIZE_T regionSize = 4096;
	cout << "Раздельное резервирование региона и передача физической памяти (режим ввода адреса)" << endl;
	cout << "Введите желаемый начальный адрес региона (hex): 0x";
	unsigned long long desiredInput;
	cin >> hex >> desiredInput;
	LPVOID desiredAddr = (LPVOID)desiredInput;
	LPVOID reservedAddr = VirtualAlloc(desiredAddr, regionSize, MEM_RESERVE, PAGE_READWRITE);
	if (reservedAddr == NULL) {
		cout << "Ошибка VirtualAlloc (MEM_RESERVE): " << GetLastError() << endl;
		return;
	}
	cout << "Регион успешно зарезервирован по адресу: 0x" << hex << (unsigned long long)reservedAddr << endl;
	cout << dec;
	LPVOID committedAddr = VirtualAlloc(reservedAddr, regionSize, MEM_COMMIT, PAGE_READWRITE);
	if (committedAddr == NULL) {
		cout << "Ошибка VirtualAlloc (MEM_COMMIT): " << GetLastError() << endl;
		VirtualFree(reservedAddr, 0, MEM_RELEASE);
		return;
	}
	cout << "Физическая память успешно передана (коммит выполнен)." << endl;
	cout << "Освободить регион сейчас? (y/n): ";
	char choice;
	cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		if (VirtualFree(reservedAddr, 0, MEM_RELEASE)) {
			cout << "Регион успешно освобожден (VirtualFree)." << endl;
		}
		else {
			cout << "Ошибка VirtualFree: " << GetLastError() << endl;
		}
	}
	else {
		cout << "Регион оставлен выделенным. Адрес: 0x" << hex << (unsigned long long)reservedAddr << endl;
		cout << dec;
	}
}

void simultaneousReserveCommitAuto() {
	const SIZE_T regionSize = 4096;
	cout << "Одновременное резервирование региона и передача физической памяти (автоматический режим)" << endl;
	LPVOID addr = VirtualAlloc(NULL, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (addr == NULL) {
		cout << "Ошибка VirtualAlloc (MEM_RESERVE | MEM_COMMIT): " << GetLastError() << endl;
		return;
	}
	cout << "Регион успешно зарезервирован и закоммичен по адресу: 0x" << hex << (unsigned long long)addr << endl;
	cout << dec;
	cout << "Освободить регион сейчас? (y/n): ";
	char choice;
	cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		if (VirtualFree(addr, 0, MEM_RELEASE)) {
			cout << "Регион успешно освобожден (VirtualFree)." << endl;
		}
		else {
			cout << "Ошибка VirtualFree: " << GetLastError() << endl;
		}
	}
	else {
		cout << "Регион оставлен выделенным. Адрес: 0x" << hex << (unsigned long long)addr << endl;
		cout << dec;
	}
}

void simultaneousReserveCommitManual() {
	const SIZE_T regionSize = 4096;
	cout << "Одновременное резервирование региона и передача физической памяти (режим ввода адреса)" << endl;
	cout << "Введите желаемый начальный адрес региона (hex): 0x";
	unsigned long long desiredInput;
	cin >> hex >> desiredInput;
	LPVOID desiredAddr = (LPVOID)desiredInput;
	LPVOID addr = VirtualAlloc(desiredAddr, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (addr == NULL) {
		cout << "Ошибка VirtualAlloc (MEM_RESERVE | MEM_COMMIT): " << GetLastError() << endl;
		return;
	}
	cout << "Регион успешно зарезервирован и закоммичен по адресу: 0x" << hex << (unsigned long long)addr << endl;
	cout << dec;
	cout << "Освободить регион сейчас? (y/n): ";
	char choice;
	cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		if (VirtualFree(addr, 0, MEM_RELEASE)) {
			cout << "Регион успешно освобожден (VirtualFree)." << endl;
		}
		else {
			cout << "Ошибка VirtualFree: " << GetLastError() << endl;
		}
	}
	else {
		cout << "Регион оставлен выделенным. Адрес: 0x" << hex << (unsigned long long)addr << endl;
		cout << dec;
	}
}

void writeToMemory() {
	cout << "Запись данных в ячейки памяти по заданному адресу" << endl;
	cout << "Введите адрес (hex): 0x";
	unsigned long long addrInput;
	cin >> hex >> addrInput;
	cout << "Введите строку данных для записи: ";
	string data;
	cin.ignore(9223372036854775807L, '\n');
	getline(cin, data);
	if (data.empty()) {
		cout << "Данные не введены." << endl;
		return;
	}
	char* target = (char*)addrInput;
	size_t len = data.length();
	for (size_t i = 0; i < len; i++) {
		target[i] = data[i];
	}
	target[len] = '\0';
	cout << "Данные успешно записаны по адресу 0x" << hex << addrInput << endl;
	cout << dec;
	cout << "Прочитано обратно для проверки: " << target << endl;
}

void setProtectionAndCheck() {
	cout << "Установка защиты доступа для региона памяти и ее проверка" << endl;
	cout << "Введите адрес начала региона (hex): 0x";
	unsigned long long addrInput;
	cin >> hex >> addrInput;
	LPVOID addr = (LPVOID)addrInput;
	cout << "Введите размер региона (байт): ";
	SIZE_T size;
	cin >> size;
	cout << "Выберите новую защиту:" << endl;
	cout << "1 - PAGE_READONLY" << endl;
	cout << "2 - PAGE_READWRITE" << endl;
	cout << "3 - PAGE_EXECUTE_READWRITE" << endl;
	cout << "Выбор: ";
	int protChoice;
	cin >> protChoice;
	DWORD newProtect = PAGE_READWRITE;
	if (protChoice == 1) newProtect = PAGE_READONLY;
	else if (protChoice == 2) newProtect = PAGE_READWRITE;
	else if (protChoice == 3) newProtect = PAGE_EXECUTE_READWRITE;
	DWORD oldProtect = 0;
	BOOL success = VirtualProtect(addr, size, newProtect, &oldProtect);
	if (success) {
		cout << "VirtualProtect успешно выполнен. Старая защита: 0x" << hex << oldProtect << endl;
		cout << dec;
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(addr, &mbi, sizeof(mbi))) {
			cout << "Текущая защита после изменения (VirtualQuery): 0x" << hex << mbi.Protect << endl;
			cout << dec;
		}
	}
	else {
		cout << "VirtualProtect вернул ошибку: " << GetLastError() << endl;
	}
}

int main() {
	setlocale(LC_ALL, "");
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	while (true) {
		cout << "=== Управление виртуальной памятью Win32 ===" << endl;
		cout << "1. Получение информации о вычислительной системе (GetSystemInfo)" << endl;
		cout << "2. Определение статуса виртуальной памяти (GlobalMemoryStatus)" << endl;
		cout << "3. Определение состояния участка памяти по адресу (VirtualQuery)" << endl;
		cout << "4. Раздельное резервирование региона и передача физической памяти (авто)" << endl;
		cout << "5. Раздельное резервирование региона и передача физической памяти (ввод адреса)" << endl;
		cout << "6. Одновременное резервирование и передача физической памяти (авто)" << endl;
		cout << "7. Одновременное резервирование и передача физической памяти (ввод адреса)" << endl;
		cout << "8. Запись данных в ячейки памяти по адресу" << endl;
		cout << "9. Установка защиты доступа региона и проверка (VirtualProtect)" << endl;
		cout << "0. Выход" << endl;
		cout << "Выберите пункт меню: ";
		int choice;
		cin >> choice;
		if (choice == 0) break;
		switch (choice) {
			case 1: showSystemInfo(); break;
			case 2: showMemoryStatus(); break;
			case 3: queryMemoryStatus(); break;
			case 4: separateReserveCommitAuto(); break;
			case 5: separateReserveCommitManual(); break;
			case 6: simultaneousReserveCommitAuto(); break;
			case 7: simultaneousReserveCommitManual(); break;
			case 8: writeToMemory(); break;
			case 9: setProtectionAndCheck(); break;
			default: cout << "Неверный пункт меню!" << endl;
		}
		system("pause");
	}
	return 0;
}