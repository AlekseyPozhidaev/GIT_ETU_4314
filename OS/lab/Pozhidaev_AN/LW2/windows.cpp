#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

// Глобальные переменные для хранения информации о зарезервированном регионе (раздельное выделение)
LPVOID g_ReservedBase = NULL;
SIZE_T g_ReservedSize = 0;
BOOL g_IsReserved = FALSE;

// Расшифровка флагов защиты (Protect)
const char* GetProtectString(DWORD protect) {
    switch (protect) {
        case PAGE_NOACCESS:          return "PAGE_NOACCESS";
        case PAGE_READONLY:          return "PAGE_READONLY";
        case PAGE_READWRITE:         return "PAGE_READWRITE";
        case PAGE_WRITECOPY:         return "PAGE_WRITECOPY";
        case PAGE_EXECUTE:           return "PAGE_EXECUTE";
        case PAGE_EXECUTE_READ:      return "PAGE_EXECUTE_READ";
        case PAGE_EXECUTE_READWRITE: return "PAGE_EXECUTE_READWRITE";
        case PAGE_EXECUTE_WRITECOPY: return "PAGE_EXECUTE_WRITECOPY";
        case PAGE_GUARD:             return "PAGE_GUARD";
        case PAGE_NOCACHE:           return "PAGE_NOCACHE";
        case PAGE_WRITECOMBINE:      return "PAGE_WRITECOMBINE";
        default:                     return "UNKNOWN";
    }
}

// Расшифровка типа региона (Type)
const char* GetTypeString(DWORD type) {
    switch (type) {
        case MEM_IMAGE:   return "MEM_IMAGE (executable image)";
        case MEM_MAPPED:  return "MEM_MAPPED (mapped file)";
        case MEM_PRIVATE: return "MEM_PRIVATE (private memory)";
        default:          return "UNKNOWN";
    }
}

// Расшифровка состояния (State)
const char* GetStateString(DWORD state) {
    switch (state) {
        case MEM_COMMIT:  return "COMMIT (physical memory allocated)";
        case MEM_RESERVE: return "RESERVE (reserved, no physical memory)";
        case MEM_FREE:    return "FREE (free)";
        default:          return "UNKNOWN";
    }
}

// 1. GetSystemInfo – добавлен вывод гранулярности
void ShowSystemInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    printf("\n=== System Information (GetSystemInfo) ===\n");
    printf("Processor architecture: %d\n", si.wProcessorArchitecture);
    printf("Page size: %lu bytes\n", si.dwPageSize);
    printf("Allocation granularity: %lu bytes\n", si.dwAllocationGranularity);
    printf("Minimum application address: 0x%p\n", si.lpMinimumApplicationAddress);
    printf("Maximum application address: 0x%p\n", si.lpMaximumApplicationAddress);
    printf("Number of processors: %lu\n", si.dwNumberOfProcessors);
    printf("Active processor mask: 0x%llX\n", si.dwActiveProcessorMask);
}

// 2. GlobalMemoryStatusEx
void ShowMemoryStatus() {
    MEMORYSTATUSEX ms = { sizeof(MEMORYSTATUSEX) };
    
    if (GlobalMemoryStatusEx(&ms)) {
        printf("\n=== Virtual Memory Status (GlobalMemoryStatusEx) ===\n");
        printf("Total physical memory:     %llu MB\n", ms.ullTotalPhys / (1024 * 1024));
        printf("Available physical memory: %llu MB\n", ms.ullAvailPhys / (1024 * 1024));
        printf("Total virtual memory:      %llu MB\n", ms.ullTotalVirtual / (1024 * 1024));
        printf("Available virtual memory:  %llu MB\n", ms.ullAvailVirtual / (1024 * 1024));
        printf("Total page file size:      %llu MB\n", ms.ullTotalPageFile / (1024 * 1024));
    } else {
        printf("GlobalMemoryStatusEx failed. Error: %lu\n", GetLastError());
    }
}

// 3. VirtualQuery – улучшенный вывод (AllocationBase, текстовые расшифровки)
void VirtualQueryDemo() {
    void* address = NULL;
    printf("\nEnter address for VirtualQuery (in hex, e.g. 0x400000 or 0x00000000): ");
    scanf("%p", &address);

    MEMORY_BASIC_INFORMATION mbi = { 0 };
    SIZE_T result = VirtualQuery(address, &mbi, sizeof(mbi));
    
    if (result > 0) {
        printf("\n=== VirtualQuery Result ===\n");
        printf("Base address of region:       0x%p\n", mbi.BaseAddress);
        printf("Allocation base:              0x%p\n", mbi.AllocationBase);
        printf("Region size:                  %zu bytes (0x%zX)\n", mbi.RegionSize, mbi.RegionSize);
        printf("State:                        %s (0x%X)\n", GetStateString(mbi.State), mbi.State);
        printf("Protect:                      %s (0x%X)\n", GetProtectString(mbi.Protect), mbi.Protect);
        printf("Type:                         %s (0x%X)\n", GetTypeString(mbi.Type), mbi.Type);
    } else {
        printf("VirtualQuery failed. Error: %lu\n", GetLastError());
    }
}

// Вспомогательная функция для проверки выравнивания адреса
BOOL IsAddressAligned(LPVOID addr, DWORD granularity) {
    return ((DWORD_PTR)addr % granularity) == 0;
}

// 4.1 Раздельное резервирование (MEM_RESERVE) – автоматический адрес
void ReserveRegionAuto() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T size = 4096 * 16; // 64 KB

    LPVOID ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    if (ptr) {
        printf("\n=== Region reserved (MEM_RESERVE) ===\n");
        printf("Reserved at address: 0x%p, size: %zu bytes\n", ptr, size);
        printf("State: MEM_RESERVE (no physical memory allocated yet)\n");
        // Сохраняем для последующего COMMIT
        if (g_ReservedBase) VirtualFree(g_ReservedBase, 0, MEM_RELEASE);
        g_ReservedBase = ptr;
        g_ReservedSize = size;
        g_IsReserved = TRUE;
    } else {
        printf("VirtualAlloc(MEM_RESERVE) failed. Error: %lu\n", GetLastError());
    }
}

// 4.2 Раздельное резервирование – ручной адрес
void ReserveRegionManual() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T size = 4096 * 16;
    LPVOID addr = NULL;
    printf("Enter base address (hex, must be multiple of %lu): ", si.dwAllocationGranularity);
    scanf("%p", &addr);

    if (!IsAddressAligned(addr, si.dwAllocationGranularity)) {
        printf("Error: Address is not aligned to allocation granularity (%lu).\n", si.dwAllocationGranularity);
        return;
    }

    LPVOID ptr = VirtualAlloc(addr, size, MEM_RESERVE, PAGE_NOACCESS);
    if (ptr) {
        printf("Region reserved at requested address: 0x%p\n", ptr);
        if (g_ReservedBase) VirtualFree(g_ReservedBase, 0, MEM_RELEASE);
        g_ReservedBase = ptr;
        g_ReservedSize = size;
        g_IsReserved = TRUE;
    } else {
        printf("VirtualAlloc(MEM_RESERVE) failed. Error: %lu (possibly address unavailable or misaligned)\n", GetLastError());
    }
}

// 5. Передача физической памяти ранее зарезервированному региону (MEM_COMMIT)
void CommitToReservedRegion() {
    if (!g_IsReserved || g_ReservedBase == NULL) {
        printf("No reserved region found. Please reserve a region first (menu items 6 or 7).\n");
        return;
    }
    // Пытаемся выделить физическую память для зарезервированного диапазона
    LPVOID ptr = VirtualAlloc(g_ReservedBase, g_ReservedSize, MEM_COMMIT, PAGE_READWRITE);
    if (ptr) {
        printf("Physical memory committed for region at 0x%p, size %zu bytes\n", ptr, g_ReservedSize);
        // Проверяем состояние через VirtualQuery
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
            printf("New state: %s\n", GetStateString(mbi.State));
        }
    } else {
        printf("VirtualAlloc(MEM_COMMIT) failed. Error: %lu\n", GetLastError());
    }
}

// 6. Одновременное резервирование+COMMIT (из исходной программы, улучшен)
void VirtualAllocDemo() {
    printf("\n=== VirtualAlloc (RESERVE | COMMIT) ===\n");
    printf("1. Automatic address (NULL)\n");
    printf("2. Manual address\n");
    int mode;
    scanf("%d", &mode);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T size = 4096 * 16;
    LPVOID addr = NULL;

    if (mode == 2) {
        printf("Enter base address (hex, must be multiple of %lu): ", si.dwAllocationGranularity);
        scanf("%p", &addr);
        if (!IsAddressAligned(addr, si.dwAllocationGranularity)) {
            printf("Error: Address not aligned. Allocation cancelled.\n");
            return;
        }
    }

    LPVOID ptr = VirtualAlloc(addr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (ptr) {
        printf("Memory allocated at: 0x%p, size %zu bytes\n", ptr, size);
        char* text = (char*)ptr;
        sprintf(text, "Hello from VirtualAlloc! Address = 0x%p", ptr);
        printf("Data written: %s\n", text);

        printf("\nPress any key to free memory (MEM_RELEASE)...");
        _getch();
        VirtualFree(ptr, 0, MEM_RELEASE);
        printf("Memory released.\n");
    } else {
        printf("VirtualAlloc failed. Error: %lu\n", GetLastError());
    }
}

// 7. Запись данных по произвольному адресу (с проверкой состояния и защиты)
void WriteDataToAddress() {
    void* address = NULL;
    DWORD value = 0;
    printf("\n=== Write data to arbitrary address ===\n");
    printf("Enter target address (hex): ");
    scanf("%p", &address);
    printf("Enter DWORD value to write (decimal): ");
    scanf("%lu", &value);

    // Проверяем состояние памяти через VirtualQuery
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        printf("VirtualQuery failed. Error: %lu\n", GetLastError());
        return;
    }

    if (mbi.State != MEM_COMMIT) {
        printf("Cannot write: memory state is %s (not COMMIT).\n", GetStateString(mbi.State));
        return;
    }

    if (!(mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) {
        printf("Cannot write: current protection %s does not allow write access.\n", GetProtectString(mbi.Protect));
        return;
    }

    // Пытаемся записать
    __try {
        *(DWORD*)address = value;
        printf("Successfully wrote value %lu to address 0x%p\n", value, address);
        printf("Verification read: %lu\n", *(DWORD*)address);
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        printf("Access violation: unable to write to address 0x%p\n", address);
    }
}

// 8. MEM_DECOMMIT – освобождение физической памяти без удаления региона
void DecommitRegion() {
    if (!g_IsReserved || g_ReservedBase == NULL) {
        printf("No reserved region found. Use menu 6 or 7 to reserve first.\n");
        return;
    }
    // Сначала убедимся, что регион в состоянии COMMIT (если нет – можно всё равно DECOMMIT, но это не повредит)
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(g_ReservedBase, &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT) {
            if (VirtualFree(g_ReservedBase, g_ReservedSize, MEM_DECOMMIT)) {
                printf("MEM_DECOMMIT succeeded. Physical memory freed, region remains reserved.\n");
                // Проверяем новое состояние
                VirtualQuery(g_ReservedBase, &mbi, sizeof(mbi));
                printf("Region state after DECOMMIT: %s\n", GetStateString(mbi.State));
            } else {
                printf("MEM_DECOMMIT failed. Error: %lu\n", GetLastError());
            }
        } else {
            printf("Region is not in COMMIT state (current: %s). Nothing to decommit.\n", GetStateString(mbi.State));
        }
    } else {
        printf("VirtualQuery failed.\n");
    }
}

// 9. VirtualProtect с проверкой записи (расширенная версия)
void VirtualProtectDemo() {
    printf("\n=== VirtualProtect with write test ===\n");
    // Выделяем память (одновременно)
    LPVOID ptr = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!ptr) {
        printf("Failed to allocate memory. Error: %lu\n", GetLastError());
        return;
    }
    printf("Memory allocated at 0x%p with PAGE_READWRITE\n", ptr);
    // Записываем тестовое значение
    *(DWORD*)ptr = 0x12345678;
    printf("Initial write successful, value = 0x%X\n", *(DWORD*)ptr);

    DWORD oldProtect = 0;
    if (VirtualProtect(ptr, 4096, PAGE_READONLY, &oldProtect)) {
        printf("Protection changed to PAGE_READONLY. Old protection: %s (0x%X)\n", GetProtectString(oldProtect), oldProtect);
    } else {
        printf("VirtualProtect failed. Error: %lu\n", GetLastError());
        VirtualFree(ptr, 0, MEM_RELEASE);
        return;
    }

    // Попытка записи после смены защиты
    printf("Attempting to write to read-only page...\n");
    __try {
        *(DWORD*)ptr = 0xDEADBEEF;
        printf("Write succeeded! (unexpected)\n");
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        printf("Write failed as expected: access violation (EXCEPTION_ACCESS_VIOLATION).\n");
    }

    // Возвращаем исходную защиту и освобождаем память
    VirtualProtect(ptr, 4096, PAGE_READWRITE, &oldProtect);
    VirtualFree(ptr, 0, MEM_RELEASE);
    printf("Memory freed.\n");
}

// Очистка глобального зарезервированного региона (если нужно перед выходом)
void CleanupReservedRegion() {
    if (g_ReservedBase) {
        VirtualFree(g_ReservedBase, 0, MEM_RELEASE);
        g_ReservedBase = NULL;
        g_IsReserved = FALSE;
    }
}

int main() {
    int choice;

    while (1) {
        system("cls");
        printf("==========================================\n");
        printf("   Lab 2.1 - Virtual Memory Management (Win32)\n");
        printf("==========================================\n");
        printf("1.  GetSystemInfo()\n");
        printf("2.  GlobalMemoryStatusEx()\n");
        printf("3.  VirtualQuery() by address\n");
        printf("4.  VirtualAlloc() (RESERVE|COMMIT) auto\n");
        printf("5.  VirtualAlloc() (RESERVE|COMMIT) manual\n");
        printf("6.  [Separate] Reserve region only (auto address)\n");
        printf("7.  [Separate] Reserve region only (manual address)\n");
        printf("8.  [Separate] Commit to reserved region\n");
        printf("9.  Write data to arbitrary address (with checks)\n");
        printf("10. [Separate] Decommit (MEM_DECOMMIT) physical memory\n");
        printf("11. VirtualProtect() with write test\n");
        printf("0.  Exit\n");
        printf("==========================================\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            choice = -1;
        }
        while (getchar() != '\n'); // очистка буфера

        switch (choice) {
            case 1: ShowSystemInfo(); break;
            case 2: ShowMemoryStatus(); break;
            case 3: VirtualQueryDemo(); break;
            case 4: VirtualAllocDemo(); break;   // автоматический адрес (совмещённый)
            case 5: VirtualAllocDemo(); break;   // ручной адрес (та же функция, но с режимом 2)
            case 6: ReserveRegionAuto(); break;
            case 7: ReserveRegionManual(); break;
            case 8: CommitToReservedRegion(); break;
            case 9: WriteDataToAddress(); break;
            case 10: DecommitRegion(); break;
            case 11: VirtualProtectDemo(); break;
            case 0:
                CleanupReservedRegion();
                printf("\nProgram terminated.\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }

        printf("\nPress any key to continue...");
        _getch();
    }
}