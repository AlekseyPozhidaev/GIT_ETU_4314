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
#define PAGE_SIZE 4096      // Размер физической страницы ОС [cite: 157, 203]
#define BUFFER_FILENAME L"buffer.bin"
#define SHARED_MEM_NAME L"Local\\MySharedMem_431417"

struct Page {
    int readerCount;
    char data[PAGE_SIZE - sizeof(int)];
};