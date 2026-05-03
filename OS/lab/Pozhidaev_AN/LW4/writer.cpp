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

    // 1. Работа с физическим файлом
    HANDLE hFile = CreateFile(BUFFER_FILENAME, GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    // Устанавливаем размер файла, если он новый
    LARGE_INTEGER fileSize;
    fileSize.QuadPart = NUM_PAGES * PAGE_SIZE;
    SetFilePointerEx(hFile, fileSize, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    // 2. Создание проецируемого файла на базе дескриптора hFile [cite: 156, 284]
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, (DWORD)fileSize.QuadPart, SHARED_MEM_NAME);
    Page* buffer = (Page*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, (DWORD)fileSize.QuadPart);
    
    VirtualLock(buffer, (SIZE_T)fileSize.QuadPart); // Блокировка в RAM [cite: 159, 286]

    // 3. Инициализация мьютексов для каждой страницы
    std::vector<HANDLE> mWriter(NUM_PAGES);
    for (int i = 0; i < NUM_PAGES; i++) {
        mWriter[i] = CreateMutex(NULL, FALSE, (L"Global\\W_Mutex_" + std::to_wstring(i)).c_str());
    }

    srand(pid + (unsigned int)time(NULL));
    std::cout << "Writer " << pid << " started using " << NUM_PAGES << " pages." << std::endl;

    for(int i = 0; i < 15; i++) {
        int p = rand() % NUM_PAGES;
        Log(log, pid, p, "WAITING"); // Регистрация начала ожидания [cite: 166, 238]
        
        WaitForSingleObject(mWriter[p], INFINITE);

        std::string content = "Writer_" + std::to_string(pid) + "_Data_" + std::to_string(rand() % 1000);
        Log(log, pid, p, "WRITING", content); // Лог записи [cite: 166, 240, 269]
        
        memset(buffer[p].data, 0, sizeof(buffer[p].data));
        strcpy_s(buffer[p].data, content.c_str());
        
        Sleep(500 + rand() % 1001); // Имитация работы 0.5-1.5 сек [cite: 163, 204, 221]

        Log(log, pid, p, "RELEASING");
        ReleaseMutex(mWriter[p]);
        Sleep(300);
    }

    // Очистка ресурсов
    VirtualUnlock(buffer, (SIZE_T)fileSize.QuadPart);
    UnmapViewOfFile(buffer);
    CloseHandle(hMap);
    CloseHandle(hFile);
    return 0;
}