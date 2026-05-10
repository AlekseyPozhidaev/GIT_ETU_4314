#include "shared_config.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

void Log(std::ofstream& log, DWORD pid, int page, const std::string& state, const std::string& data = "-") {
    log << timeGetTime() << "," << pid << ",WRITER," << page << "," << state << "," << data << std::endl;
}

int main() {
    DWORD pid = GetCurrentProcessId();
    std::ofstream log("writer_" + std::to_string(pid) + ".csv");
    log << "TIME,PID,ROLE,PAGE,STATE,DATA" << std::endl;

    // Открываем/создаём файл
    HANDLE hFile = CreateFile(BUFFER_FILENAME, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    LARGE_INTEGER fileSize;
    fileSize.QuadPart = (LONGLONG)NUM_PAGES * PAGE_SIZE;
    SetFilePointerEx(hFile, fileSize, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    // Проецируем файл
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, (DWORD)fileSize.QuadPart, SHARED_MEM_NAME);
    Page* buffer = (Page*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)fileSize.QuadPart);

    VirtualLock(buffer, (SIZE_T)fileSize.QuadPart);

    // Мьютексы
    std::vector<HANDLE> mWriter(NUM_PAGES);
    std::vector<HANDLE> mReader(NUM_PAGES);
    for (int i = 0; i < NUM_PAGES; i++) {
        mWriter[i] = CreateMutex(NULL, FALSE, (L"Global\\W_Mutex_" + std::to_wstring(i)).c_str());
        mReader[i] = CreateMutex(NULL, FALSE, (L"Global\\R_Mutex_" + std::to_wstring(i)).c_str());
    }

    srand(pid + (unsigned int)time(NULL));
    std::cout << "Writer " << pid << " started using " << NUM_PAGES << " pages." << std::endl;

    for (int i = 0; i < OPERATIONS; i++) {          // Увеличил количество итераций
        int p = rand() % NUM_PAGES;

        Log(log, pid, p, "WAITING");

        // === КРИТИЧЕСКАЯ СЕКЦИЯ ПИСАТЕЛЯ ===
        WaitForSingleObject(mWriter[p], INFINITE);

        LONG newVersion = InterlockedIncrement(&buffer[p].version);

        std::string content = "Writer_" + std::to_string(pid) + "_V" + std::to_string(newVersion) +
                             "_Data_" + std::to_string(rand() % 10000);

        Log(log, pid, p, "WRITING", content);

        memset(buffer[p].data, 0, sizeof(buffer[p].data));
        strcpy_s(buffer[p].data, content.c_str());

        Sleep(500 + rand() % 1001);   // 0.5 — 1.5 сек

        Log(log, pid, p, "RELEASING");
        ReleaseMutex(mWriter[p]);
        // ============================

        Sleep(200 + rand() % 400);    // небольшая пауза между циклами
    }

    VirtualUnlock(buffer, (SIZE_T)fileSize.QuadPart);
    UnmapViewOfFile(buffer);
    CloseHandle(hMap);
    CloseHandle(hFile);

    std::cout << "Writer " << pid << " finished." << std::endl;
    return 0;
}