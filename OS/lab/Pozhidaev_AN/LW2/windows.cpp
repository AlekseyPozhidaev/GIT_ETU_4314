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

// 1. GetSystemInfo
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

// 2. GlobalMemoryStatus (строго по заданию)
void ShowMemoryStatus() {
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);

    printf("\n=== Virtual Memory Status (GlobalMemoryStatus) ===\n");
    printf("Total physical memory:     %lu MB\n", ms.dwTotalPhys / (1024 * 1024));
    printf("Available physical memory: %lu MB\n", ms.dwAvailPhys / (1024 * 1024));
    printf("Total virtual memory:      %lu MB\n", ms.dwTotalVirtual / (1024 * 1024));
    printf("Available virtual memory:  %lu MB\n", ms.dwAvailVirtual / (1024 * 1024));
    printf("Total page file size:      %lu MB\n", ms.dwTotalPageFile / (1024 * 1024));
}

// 3. VirtualQuery
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

// 4. Одновременное резервирование+COMMIT (автоматический адрес)
void VirtualAllocAuto() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T size = 4096 * 16; // 64 KB

    LPVOID ptr = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (ptr) {
        printf("\n=== VirtualAlloc (RESERVE | COMMIT) auto address ===\n");
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

// 5. Одновременное резервирование+COMMIT (ручной адрес)
void VirtualAllocManual() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T size = 4096 * 16;
    LPVOID addr = NULL;
    printf("Enter base address (hex, must be multiple of %lu): ", si.dwAllocationGranularity);
    scanf("%p", &addr);

    if (!IsAddressAligned(addr, si.dwAllocationGranularity)) {
        printf("Error: Address not aligned. Allocation cancelled.\n");
        return;
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
        printf("VirtualAlloc failed. Error: %lu (possibly address unavailable or misaligned)\n", GetLastError());
    }
}

// 6. Раздельное резервирование – автоматический адрес
void ReserveRegionAuto() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T size = 4096 * 16; // 64 KB

    LPVOID ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    if (ptr) {
        printf("\n=== Region reserved (MEM_RESERVE) auto address ===\n");
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

// 7. Раздельное резервирование – ручной адрес
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

// 8. Передача физической памяти ранее зарезервированному региону (MEM_COMMIT)
void CommitToReservedRegion() {
    if (!g_IsReserved || g_ReservedBase == NULL) {
        printf("No reserved region found. Please reserve a region first (menu items 6 or 7).\n");
        return;
    }
    LPVOID ptr = VirtualAlloc(g_ReservedBase, g_ReservedSize, MEM_COMMIT, PAGE_READWRITE);
    if (ptr) {
        printf("Physical memory committed for region at 0x%p, size %zu bytes\n", ptr, g_ReservedSize);
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
            printf("New state: %s\n", GetStateString(mbi.State));
        }
    } else {
        printf("VirtualAlloc(MEM_COMMIT) failed. Error: %lu\n", GetLastError());
    }
}

// 9. Запись данных по произвольному адресу (с проверкой состояния и защиты)
void WriteDataToAddress() {
    void* address = NULL;
    DWORD value = 0;
    printf("\n=== Write data to arbitrary address ===\n");
    printf("Enter target address (hex): ");
    scanf("%p", &address);
    printf("Enter DWORD value to write (decimal): ");
    scanf("%lu", &value);

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

    // Запись (без SEH – проверки гарантируют безопасность)
    *(DWORD*)address = value;
    printf("Successfully wrote value %lu to address 0x%p\n", value, address);
    printf("Verification read: %lu\n", *(DWORD*)address);
}

// 10. MEM_DECOMMIT – освобождение физической памяти без удаления региона
void DecommitRegion() {
    if (!g_IsReserved || g_ReservedBase == NULL) {
        printf("No reserved region found. Use menu 6 or 7 to reserve first.\n");
        return;
    }
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(g_ReservedBase, &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT) {
            if (VirtualFree(g_ReservedBase, g_ReservedSize, MEM_DECOMMIT)) {
                printf("MEM_DECOMMIT succeeded. Physical memory freed, region remains reserved.\n");
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

// 11. VirtualProtect – для заданного с клавиатуры региона
void VirtualProtectDemo() {
    void* address = NULL;
    DWORD newProtect = 0;
    printf("\n=== VirtualProtect with write test ===\n");
    printf("Enter target address (hex): ");
    scanf("%p", &address);

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        printf("VirtualQuery failed. Error: %lu\n", GetLastError());
        return;
    }

    if (mbi.State != MEM_COMMIT) {
        printf("Cannot change protection: memory state is %s (not COMMIT).\n", GetStateString(mbi.State));
        return;
    }

    printf("Current protection: %s\n", GetProtectString(mbi.Protect));
    printf("Enter new protection (numeric code, e.g. 2 for PAGE_READONLY, 4 for PAGE_READWRITE):\n");
    printf("2 - PAGE_READONLY\n4 - PAGE_READWRITE\n1 - PAGE_NOACCESS\n");
    scanf("%lu", &newProtect);

    DWORD oldProtect = 0;
    if (VirtualProtect(address, mbi.RegionSize, newProtect, &oldProtect)) {
        printf("Protection changed. Old protection: %s (0x%X)\n", GetProtectString(oldProtect), oldProtect);
        printf("Attempting to write to address 0x%p...\n", address);
        
        // Проверяем, разрешена ли теперь запись
        MEMORY_BASIC_INFORMATION mbi2;
        VirtualQuery(address, &mbi2, sizeof(mbi2));
        if (mbi2.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
            // Запись разрешена – выполняем
            *(DWORD*)address = 0x12345678;
            printf("Write succeeded! New value at address: 0x%X\n", *(DWORD*)address);
        } else {
            // Запись не разрешена – выводим сообщение (без SEH, чтобы не падать)
            printf("Write is not allowed due to protection %s. Access violation would occur.\n", GetProtectString(mbi2.Protect));
        }
        // Возвращаем старую защиту
        VirtualProtect(address, mbi.RegionSize, oldProtect, &oldProtect);
        printf("Protection restored to %s\n", GetProtectString(oldProtect));
    } else {
        printf("VirtualProtect failed. Error: %lu\n", GetLastError());
    }
}

// Очистка глобального зарезервированного региона
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
        printf("2.  GlobalMemoryStatus()\n");
        printf("3.  VirtualQuery() by address\n");
        printf("4.  VirtualAlloc() RESERVE|COMMIT (auto address)\n");
        printf("5.  VirtualAlloc() RESERVE|COMMIT (manual address)\n");
        printf("6.  [Separate] Reserve region only (auto address)\n");
        printf("7.  [Separate] Reserve region only (manual address)\n");
        printf("8.  [Separate] Commit to reserved region\n");
        printf("9.  Write data to arbitrary address (with checks)\n");
        printf("10. [Separate] Decommit (MEM_DECOMMIT) physical memory\n");
        printf("11. VirtualProtect() with write test (user-specified address)\n");
        printf("0.  Exit\n");
        printf("==========================================\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            choice = -1;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: ShowSystemInfo(); break;
            case 2: ShowMemoryStatus(); break;
            case 3: VirtualQueryDemo(); break;
            case 4: VirtualAllocAuto(); break;
            case 5: VirtualAllocManual(); break;
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