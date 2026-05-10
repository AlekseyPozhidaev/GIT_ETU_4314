#include "shared_config.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

void Log(std::ofstream& log, DWORD pid, int page, const std::string& state, const std::string& data = "-") {
    log << timeGetTime() << "," << pid << ",READER," << page << "," << state << "," << data << std::endl;
}

int main() {
    DWORD pid = GetCurrentProcessId();
    std::ofstream log("reader_" + std::to_string(pid) + ".csv");
    log << "TIME,PID,ROLE,PAGE,STATE,DATA" << std::endl;

    HANDLE hFile = CreateFile(BUFFER_FILENAME, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, NUM_PAGES * PAGE_SIZE, SHARED_MEM_NAME);
    Page* buffer = (Page*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, NUM_PAGES * PAGE_SIZE);

    VirtualLock(buffer, NUM_PAGES * PAGE_SIZE);

    std::vector<HANDLE> mWriter(NUM_PAGES);
    std::vector<HANDLE> mReader(NUM_PAGES);
    for (int i = 0; i < NUM_PAGES; i++) {
        mWriter[i] = CreateMutex(NULL, FALSE, (L"Global\\W_Mutex_" + std::to_wstring(i)).c_str());
        mReader[i] = CreateMutex(NULL, FALSE, (L"Global\\R_Mutex_" + std::to_wstring(i)).c_str());
    }

    srand(pid + (unsigned int)time(NULL));
    std::cout << "Reader " << pid << " started." << std::endl;

    for (int i = 0; i < OPERATIONS; i++) {        // Больше итераций у читателей
        int p = rand() % NUM_PAGES;

        Log(log, pid, p, "WAITING");

        // === ВХОД ЧИТАТЕЛЯ (приоритет читателей) ===
        WaitForSingleObject(mReader[p], INFINITE);
        if (++buffer[p].readerCount == 1) {
            WaitForSingleObject(mWriter[p], INFINITE);   // Первый читатель блокирует писателей
        }
        ReleaseMutex(mReader[p]);
        // =========================================

        LONG currentVersion = buffer[p].version;
        std::string content(buffer[p].data);

        Log(log, pid, p, "READING", "V" + std::to_string(currentVersion) + ":" +
                          (content.empty() ? "EMPTY" : content));

        Sleep(500 + rand() % 1001);

        Log(log, pid, p, "RELEASING");

        // === ВЫХОД ЧИТАТЕЛЯ ===
        WaitForSingleObject(mReader[p], INFINITE);
        if (--buffer[p].readerCount == 0) {
            ReleaseMutex(mWriter[p]);   // Последний читатель освобождает писателей
        }
        ReleaseMutex(mReader[p]);
        // ======================

        Sleep(150 + rand() % 300);
    }

    VirtualUnlock(buffer, NUM_PAGES * PAGE_SIZE);
    UnmapViewOfFile(buffer);
    CloseHandle(hMap);
    CloseHandle(hFile);

    std::cout << "Reader " << pid << " finished." << std::endl;
    return 0;
}