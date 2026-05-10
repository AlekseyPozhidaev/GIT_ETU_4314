#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <string>

#define STUDENT_ID "431417"
#define NUM_PAGES 16        // 3+1+4+1+7 = 16
#define PAGE_SIZE 4096      // Размер физической страницы

#define BUFFER_FILENAME L"buffer.bin"
#define SHARED_MEM_NAME L"Local\\MySharedMem_431417"
#define OPERATIONS 50

struct Page {
    int  readerCount;     // Счётчик активных читателей
    LONG version;         // Версия данных (увеличивается при каждой записи)
    char data[PAGE_SIZE - sizeof(int) - sizeof(LONG)];
};