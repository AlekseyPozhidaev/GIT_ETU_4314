#include <windows.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <clocale>

using namespace std;

void printFileTime(const char* label, const FILETIME& ft) {
    if (ft.dwLowDateTime == 0 && ft.dwHighDateTime == 0) {
        cout << label << ": not set" << endl;
        return;
    }
    FILETIME localFT;
    SYSTEMTIME st;
    FileTimeToLocalFileTime(&ft, &localFT);
    FileTimeToSystemTime(&localFT, &st);
    cout << label << ": "
         << st.wYear << "-"
         << setw(2) << setfill('0') << st.wMonth << "-"
         << setw(2) << setfill('0') << st.wDay << " "
         << setw(2) << setfill('0') << st.wHour << ":"
         << setw(2) << setfill('0') << st.wMinute << ":"
         << setw(2) << setfill('0') << st.wSecond << endl;
}

void listDrives() {
    cout << "\n=== 1. List of drives ===" << endl;

    DWORD driveMask = GetLogicalDrives();
    cout << "GetLogicalDrives(): bit mask = 0x" << hex << driveMask << dec << endl;
    cout << "Available drives:" << endl;
    for (char c = 'A'; c <= 'Z'; ++c) {
        if (driveMask & (1 << (c - 'A'))) {
            cout << "  " << c << ":\\" << endl;
        }
    }

    char buffer[1024] = {0};
    DWORD len = GetLogicalDriveStringsA(sizeof(buffer), buffer);
    if (len > 0) {
        cout << "\nGetLogicalDriveStrings():" << endl;
        const char* p = buffer;
        while (*p) {
            cout << "  " << p << endl;
            p += strlen(p) + 1;
        }
    }
}

void driveInfo() {
    cout << "\n=== 2. Drive information ===" << endl;

    string drive;
    cout << "Enter drive (e.g. C:\\): ";
    getline(cin, drive);
    if (!drive.empty() && drive.back() != '\\') drive += '\\';

    UINT type = GetDriveTypeA(drive.c_str());
    cout << "GetDriveType(): ";
    switch (type) {
        case DRIVE_UNKNOWN:     cout << "Unknown"; break;
        case DRIVE_NO_ROOT_DIR: cout << "No root directory"; break;
        case DRIVE_REMOVABLE:   cout << "Removable (USB, etc.)"; break;
        case DRIVE_FIXED:       cout << "Fixed (HDD/SSD)"; break;
        case DRIVE_REMOTE:      cout << "Network drive"; break;
        case DRIVE_CDROM:       cout << "CD/DVD/Blu-ray"; break;
        case DRIVE_RAMDISK:     cout << "RAM disk"; break;
        default:                cout << "Code " << type;
    }
    cout << endl;

    char volName[MAX_PATH + 1] = {0};
    char fsName[MAX_PATH + 1] = {0};
    DWORD serial = 0, maxComp = 0, flags = 0;
    GetVolumeInformationA(drive.c_str(), volName, MAX_PATH, &serial, &maxComp, &flags, fsName, MAX_PATH);
    cout << "GetVolumeInformation():\n"
    << "  Volume label: " << (volName[0] ? volName : "<none>") << "\n"
    << "  Serial number: " << serial << "\n"
    << "  File system: " << fsName << endl;

    ULARGE_INTEGER freeBytesAvail, totalBytes, totalFreeBytes;
    GetDiskFreeSpaceExA(drive.c_str(), &freeBytesAvail, &totalBytes, &totalFreeBytes);
    cout << "GetDiskFreeSpaceEx():\n"
         << "  Free bytes available to caller: " 
         << freeBytesAvail.QuadPart / (1024ULL * 1024) << " MB\n"
         << "  Total free bytes on disk:       " 
         << totalFreeBytes.QuadPart / (1024ULL * 1024) << " MB\n"
         << "  Total size of volume:           " 
         << totalBytes.QuadPart / (1024ULL * 1024) << " MB" << endl;

    DWORD spc = 0, bps = 0, freeCl = 0, totalCl = 0;
    GetDiskFreeSpaceA(drive.c_str(), &spc, &bps, &freeCl, &totalCl);
    cout << "  Cluster size (from GetDiskFreeSpace): " 
         << (ULONGLONG)spc * bps << " bytes" << endl;
}

void createDirectoryFunc() {
    cout << "\n=== 3. Create directory ===" << endl;
    string path;
    cout << "Full path of new directory: ";
    getline(cin, path);
    if (CreateDirectoryA(path.c_str(), NULL)) {
        cout << "Directory created successfully." << endl;
    } else {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            cout << "Directory already exists." << endl;
        }
    }
}

void removeDirectoryFunc() {
    cout << "\n=== 4. Remove directory ===" << endl;
    string path;
    cout << "Full path of directory to remove: ";
    getline(cin, path);

    if (path.empty()) {
        cout << "No path entered." << endl;
        return;
    }

    if (RemoveDirectoryA(path.c_str())) {
        cout << "Directory removed successfully." << endl;
    } else {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            cout << "Directory does not exist." << endl;
        } else if (err == ERROR_DIR_NOT_EMPTY) {
            cout << "Cannot remove: directory is not empty." << endl;
        } else if (err == ERROR_ACCESS_DENIED) {
            cout << "Access denied." << endl;
        } else {
            cout << "Failed to remove directory. Error code: " << err << endl;
        }
    }
}

void createFileFunc() {
    cout << "\n=== 5. Create file ===" << endl;
    string path;
    cout << "Full path of new file: ";
    getline(cin, path);
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        const char* testData = "Test file created via CreateFile Win32 API.\r\n";
        DWORD written;
        WriteFile(h, testData, (DWORD)strlen(testData), &written, NULL);
        CloseHandle(h);
        cout << "File created successfully with test data." << endl;
    } else {
        cout << "File created error." << endl;
    }
}

void copyFileFunc() {
    cout << "\n=== 6. Copy file ===" << endl;
    string src, dst;

    cout << "Source file (full path): ";
    getline(cin, src);

    cout << "Destination (full path): ";
    getline(cin, dst);

    if (src.empty() || dst.empty()) {
        cout << "Error: source or destination path is empty." << endl;
        return;
    }

    DWORD srcAttr = GetFileAttributesA(src.c_str());
    if (srcAttr == INVALID_FILE_ATTRIBUTES) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            cout << "Source file does not exist." << endl;
        } else {
            cout << "Cannot access source file. Error code: " << err << endl;
        }
    }

    else{
        if (CopyFileA(src.c_str(), dst.c_str(), TRUE)) {
            cout << "File copied successfully." << endl;
        } else {
            DWORD err = GetLastError();
            switch (err) {
                case ERROR_FILE_EXISTS:
                    cout << "*** File with the same name already exists at destination! ***" << endl;
                    cout << "CopyFile did not overwrite the existing file." << endl;
                    break;
                case ERROR_ACCESS_DENIED:
                    cout << "Access denied. Check permissions for source or destination." << endl;
                    break;
                case ERROR_PATH_NOT_FOUND:
                    cout << "Destination path not found" << endl;
                    break;
                case ERROR_INVALID_NAME:
                    cout << "Invalid filename or path." << endl;
                    break;
                default:
                    cout << "CopyFile failed" << endl;
                    break;
            }
        }
    }
}

void moveFileFunc() {
    cout << "\n=== 7. Move / Rename file ===" << endl;
    string src, dst;
    cout << "Source file: ";
    getline(cin, src);
    cout << "New path: ";
    getline(cin, dst);

    if (GetFileAttributesA(dst.c_str()) != INVALID_FILE_ATTRIBUTES) {
        cout << "File with the same name already exists" << endl;
        cout << "Replace existing file? (y/n): ";
        string ans;
        getline(cin, ans);
        if (ans == "y" || ans == "Y") {
            if (MoveFileExA(src.c_str(), dst.c_str(), MOVEFILE_REPLACE_EXISTING)) {
                cout << "File moved with replacement." << endl;
            }
        }
        return;
    }

    else {
        MoveFileA(src.c_str(), dst.c_str());
        cout << "File moved successfully." << endl;
    }
}

void analyzeAttributes() {
    cout << "\n=== 8. Analyze file attributes ===" << endl;
    string path;
    cout << "File or directory path: ";
    getline(cin, path);

    DWORD attr = GetFileAttributesA(path.c_str());

    cout << "GetFileAttributes() returned: 0x" << hex << attr << dec << endl;

    cout << "Attributes:" << endl;
    if (attr & FILE_ATTRIBUTE_READONLY)    cout << " Read-only" << endl;
    if (attr & FILE_ATTRIBUTE_HIDDEN)      cout << " Hidden" << endl;
    if (attr & FILE_ATTRIBUTE_SYSTEM)      cout << " System" << endl;
    if (attr & FILE_ATTRIBUTE_DIRECTORY)   cout << " Directory" << endl;
    if (attr & FILE_ATTRIBUTE_ARCHIVE)     cout << " Archive" << endl;
    if (attr & FILE_ATTRIBUTE_NORMAL)      cout << " Normal" << endl;
    if (attr & FILE_ATTRIBUTE_COMPRESSED)  cout << " Compressed" << endl;
    if (attr & FILE_ATTRIBUTE_ENCRYPTED)   cout << " Encrypted" << endl;
    if (attr & FILE_ATTRIBUTE_SPARSE_FILE) cout << " Sparse file" << endl;
    if (attr & FILE_ATTRIBUTE_TEMPORARY)   cout << " Temporary" << endl;
}

void changeAttributes() {
    cout << "\n=== 9. Change file attributes ===" << endl;
    string path;
    cout << "File path: ";
    getline(cin, path);

    DWORD cur = GetFileAttributesA(path.c_str());
    cout << "Current attributes: 0x" << hex << cur << dec << endl;

    cout << "New attributes value (e.g. 1=readonly, 32=archive, 0=normal): ";
    string s;
    getline(cin, s);
    DWORD newAttr = 0;
    newAttr = stoul(s, nullptr, 0);

    SetFileAttributesA(path.c_str(), newAttr);

    cout << "Attributes changed successfully." << endl;
}
void fileInfoByHandle() {
    cout << "\n=== 10. GetFileInformationByHandle ===" << endl;
    string path;
    cout << "File path: ";
    getline(cin, path);

    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    BY_HANDLE_FILE_INFORMATION info;
    GetFileInformationByHandle(h, &info);
    cout << "dwFileAttributes = 0x" << hex << info.dwFileAttributes << dec << endl;
    ULONGLONG size = ((ULONGLONG)info.nFileSizeHigh << 32) | info.nFileSizeLow;
    cout << "File size: " << size << " bytes" << endl;
    cout << "Volume serial number: " << info.dwVolumeSerialNumber << endl;
    cout << "File index: " << info.nFileIndexHigh << ":" << info.nFileIndexLow << endl;
    cout << "Number of hard links: " << info.nNumberOfLinks << endl;
    CloseHandle(h);
}

void getFileTimeFunc() {
        cout << "\n=== 11. Get file times (GetFileTime) ===" << endl;
    string path;
    cout << "File path: ";
    getline(cin, path);

    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, 0, NULL);

    FILETIME ct, at, wt;
    GetFileTime(h, &ct, &at, &wt);
    printFileTime("Creation time", ct);
    printFileTime("Last access time", at);
    printFileTime("Last write time", wt);
    CloseHandle(h);
}
void setFileTimeFunc() {
        cout << "\n=== 12. Set file times (SetFileTime) ===" << endl;
    string path;
    cout << "File path: ";
    getline(cin, path);

    HANDLE h = CreateFileA(path.c_str(), FILE_WRITE_ATTRIBUTES, 0,
                           NULL, OPEN_EXISTING, 0, NULL);

    FILETIME now;
    GetSystemTimeAsFileTime(&now);

    cout << "Set current time to:\n"
         << "1 - creation   2 - last access   3 - last write   4 - all three\n"
         << "Choice: ";
    string s;
    getline(cin, s);
    int choice = 0;
    try { choice = stoi(s); } catch (...) {}

    BOOL ok = FALSE;
    switch (choice) {
        case 1: ok = SetFileTime(h, &now, NULL, NULL); break;
        case 2: ok = SetFileTime(h, NULL, &now, NULL); break;
        case 3: ok = SetFileTime(h, NULL, NULL, &now); break;
        case 4: ok = SetFileTime(h, &now, &now, &now); break;
        default: cout << "Invalid choice." << endl;
    }

    if (ok) {
        cout << "File time updated successfully." << endl;
    }
    CloseHandle(h);
}

int main() {
    setlocale(LC_ALL, "");

    cout << "Console application for managing drives, folders and files (Win32 API)\n"
         << "Task 1.1\n\n";

    while (true) {
        cout << "================ MENU ================" << endl;
        cout << "1  - List drives (GetLogicalDrives / GetLogicalDriveStrings)" << endl;
        cout << "2  - Drive info + free space" << endl;
        cout << "3  - Create directory (CreateDirectory)" << endl;
        cout << "4  - Remove directory (RemoveDirectory)" << endl;
        cout << "5  - Create file (CreateFile)" << endl;
        cout << "6  - Copy file (CopyFile + name conflict detection)" << endl;
        cout << "7  - Move / Rename file (MoveFile / MoveFileEx)" << endl;
        cout << "8  - View file attributes (GetFileAttributes)" << endl;
        cout << "9  - Change file attributes (SetFileAttributes)" << endl;
        cout << "10 - File info by handle (GetFileInformationByHandle)" << endl;
        cout << "11 - Get file times (GetFileTime)" << endl;
        cout << "12 - Set file times (SetFileTime)" << endl;
        cout << "0  - Exit" << endl;
        cout << "======================================" << endl;
        cout << "Your choice: ";

        string input;
        getline(cin, input);
        if (input.empty()) continue;

        int choice = 0;
        try { choice = stoi(input); } catch (...) { choice = -1; }

        switch (choice) {
            case 1:  listDrives(); break;
            case 2:  driveInfo(); break;
            case 3:  createDirectoryFunc(); break;
            case 4:  removeDirectoryFunc(); break;
            case 5:  createFileFunc(); break;
            case 6:  copyFileFunc(); break;
            case 7:  moveFileFunc(); break;
            case 8:  analyzeAttributes(); break;
            case 9:  changeAttributes(); break;
            case 10: fileInfoByHandle(); break;
            case 11: getFileTimeFunc(); break;
            case 12: setFileTimeFunc(); break;
            case 0:  cout << "Program terminated." << endl; return 0;
            default: cout << "Invalid menu item!" << endl;
        }
    }
}