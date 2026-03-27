#include <windows.h>
#include <stdio.h>
#include <conio.h>

void ShowSystemInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    printf("\n=== Информация о системе (GetSystemInfo) ===\n");
    printf("Архитектура процессора: %d\n", si.wProcessorArchitecture);
    printf("Размер страницы: %lu байт\n", si.dwPageSize);
    printf("Минимальный адрес приложения: 0x%p\n", si.lpMinimumApplicationAddress);
    printf("Максимальный адрес приложения: 0x%p\n", si.lpMaximumApplicationAddress);
    printf("Количество процессоров: %lu\n", si.dwNumberOfProcessors);
    printf("Активная маска процессоров: 0x%llX\n", si.dwActiveProcessorMask);
}

void ShowMemoryStatus() {
    MEMORYSTATUSEX ms = { sizeof(MEMORYSTATUSEX) };
    
    if (GlobalMemoryStatusEx(&ms)) {
        printf("\n=== Статус виртуальной памяти (GlobalMemoryStatusEx) ===\n");
        printf("Общая физическая память:     %llu МБ\n", ms.ullTotalPhys / (1024 * 1024));
        printf("Доступная физическая память: %llu МБ\n", ms.ullAvailPhys / (1024 * 1024));
        printf("Общая виртуальная память:    %llu МБ\n", ms.ullTotalVirtual / (1024 * 1024));
        printf("Доступная виртуальная память:%llu МБ\n", ms.ullAvailVirtual / (1024 * 1024));
        printf("Общий размер файла подкачки: %llu МБ\n", ms.ullTotalPageFile / (1024 * 1024));
    } else {
        printf("GlobalMemoryStatusEx failed. Error: %lu\n", GetLastError());
    }
}

void VirtualQueryDemo() {
    void* address = NULL;
    printf("\nВведите адрес для VirtualQuery (в hex, например 0x400000 или 0x00000000): ");
    scanf("%p", &address);

    MEMORY_BASIC_INFORMATION mbi = { 0 };

    SIZE_T result = VirtualQuery(address, &mbi, sizeof(mbi));
    if (result > 0) {
        printf("\n=== Результат VirtualQuery ===\n");
        printf("Базовый адрес региона:  0x%p\n", mbi.BaseAddress);
        printf("Размер региона:         %zu байт (0x%zX)\n", mbi.RegionSize, mbi.RegionSize);
        printf("Состояние:              ");
        switch (mbi.State) {
            case MEM_COMMIT:  printf("COMMIT (физическая память выделена)\n"); break;
            case MEM_RESERVE: printf("RESERVE (зарезервировано, без физической памяти)\n"); break;
            case MEM_FREE:    printf("FREE (свободно)\n"); break;
            default:          printf("Неизвестно (0x%X)\n", mbi.State);
        }
        printf("Защита:                 0x%X\n", mbi.Protect);
        printf("Тип:                    0x%X\n", mbi.Type);
    } else {
        printf("VirtualQuery failed. Error: %lu\n", GetLastError());
    }
}

void VirtualAllocDemo() {
    printf("\n=== VirtualAlloc / VirtualFree ===\n");
    printf("1. Автоматический адрес (NULL)\n");
    printf("2. Указать адрес вручную\n");
    int mode;
    scanf("%d", &mode);

    SIZE_T size = 4096 * 16; // 64 КБ (16 страниц)
    LPVOID addr = NULL;

    if (mode == 2) {
        printf("Введите начальный адрес (hex): ");
        scanf("%p", &addr);
    }

    // Резервирование + Commit одновременно
    LPVOID ptr = VirtualAlloc(addr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    if (ptr) {
        printf("Успешно выделено по адресу: 0x%p\n", ptr);
        
        // Запись тестовых данных
        char* text = (char*)ptr;
        sprintf(text, "Привет из VirtualAlloc! Адрес = 0x%p", ptr);
        printf("Записано в память: %s\n", text);

        printf("\nНажмите любую клавишу для освобождения памяти...");
        _getch();

        VirtualFree(ptr, 0, MEM_RELEASE);
        printf("Память освобождена (MEM_RELEASE).\n");
    } else {
        printf("VirtualAlloc не удался. Ошибка: %lu\n", GetLastError());
    }
}

void VirtualProtectDemo() {
    printf("\n=== VirtualProtect ===\n");

    // Выделяем регион
    LPVOID ptr = VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
    if (!ptr) {
        printf("Не удалось выделить память. Ошибка: %lu\n", GetLastError());
        return;
    }

    printf("Память выделена по адресу: 0x%p с правами PAGE_READWRITE\n", ptr);

    DWORD oldProtect = 0;
    if (VirtualProtect(ptr, 4096, PAGE_READONLY, &oldProtect)) {
        printf("Защита успешно изменена на PAGE_READONLY\n");
        printf("Старая защита: 0x%X\n", oldProtect);
    } else {
        printf("VirtualProtect failed. Error: %lu\n", GetLastError());
    }

    // Проверка текущей защиты через VirtualQuery
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(ptr, &mbi, sizeof(mbi));
    printf("Текущая защита после изменения: 0x%X\n", mbi.Protect);

    VirtualFree(ptr, 0, MEM_RELEASE);
    printf("Память освобождена.\n");
}

int main() {
    int choice;

    while (true) {
        system("cls");
        printf("==========================================\n");
        printf("   Задание 2.1 - Управление виртуальной памятью (Win32)\n");
        printf("==========================================\n");
        printf("1. GetSystemInfo()\n");
        printf("2. GlobalMemoryStatusEx()\n");
        printf("3. VirtualQuery() по адресу\n");
        printf("4. VirtualAlloc() + VirtualFree()\n");
        printf("5. VirtualProtect()\n");
        printf("0. Выход\n");
        printf("==========================================\n");
        printf("Выберите пункт: ");

        if (scanf("%d", &choice) != 1) {
            choice = -1;
        }
        while (getchar() != '\n'); // очистка буфера

        switch (choice) {
            case 1: ShowSystemInfo(); break;
            case 2: ShowMemoryStatus(); break;
            case 3: VirtualQueryDemo(); break;
            case 4: VirtualAllocDemo(); break;
            case 5: VirtualProtectDemo(); break;
            case 0:
                printf("\nПрограмма завершена.\n");
                return 0;
            default:
                printf("Неверный выбор!\n");
        }

        printf("\nНажмите любую клавишу для продолжения...");
        _getch();
    }
}