#include "message_queue.h"
#include <vector>

static bool SetFileOffset(HANDLE hFile, LONGLONG offset) {
    LARGE_INTEGER li;
    li.QuadPart = offset;
    return SetFilePointerEx(hFile, li, NULL, FILE_BEGIN);
}

static bool ReadHeader(HANDLE hFile, QueueHeader& hdr) {
    SetFileOffset(hFile, 0);
    DWORD read = 0;
    return ReadFile(hFile, &hdr, sizeof(hdr), &read, NULL) && read == sizeof(hdr);
}

static bool WriteHeader(HANDLE hFile, const QueueHeader& hdr) {
    SetFileOffset(hFile, 0);
    DWORD written = 0;
    return WriteFile(hFile, &hdr, sizeof(hdr), &written, NULL) && written == sizeof(hdr);
}

bool InitMessageFile(const std::wstring& filename, int capacity) {
    HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    QueueHeader hdr = { capacity, 0, 0 };
    DWORD written = 0;
    WriteFile(hFile, &hdr, sizeof(hdr), &written, NULL);

    std::vector<char> empty(MAX_MSG_LEN, 0);
    for (int i = 0; i < capacity; i++) {
        WriteFile(hFile, empty.data(), MAX_MSG_LEN, &written, NULL);
    }
    CloseHandle(hFile);
    return true;
}

bool ReadMessage(const std::wstring& filename, std::string& out) {
    HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    QueueHeader hdr;
    if (!ReadHeader(hFile, hdr)) { CloseHandle(hFile); return false; }

    SetFileOffset(hFile, sizeof(QueueHeader) + (LONGLONG)hdr.readPos * MAX_MSG_LEN);
    char buf[MAX_MSG_LEN] = { 0 };
    DWORD read = 0;
    ReadFile(hFile, buf, MAX_MSG_LEN, &read, NULL);
    out = std::string(buf);

    hdr.readPos = (hdr.readPos + 1) % hdr.capacity;
    WriteHeader(hFile, hdr);
    CloseHandle(hFile);
    return true;
}

bool WriteMessage(const std::wstring& filename, const std::string& msg) {
    HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    QueueHeader hdr;
    if (!ReadHeader(hFile, hdr)) { CloseHandle(hFile); return false; }

    SetFileOffset(hFile, sizeof(QueueHeader) + (LONGLONG)hdr.writePos * MAX_MSG_LEN);
    char buf[MAX_MSG_LEN] = { 0 };
    memcpy(buf, msg.c_str(), (msg.size() > MAX_MSG_LEN - 1) ? MAX_MSG_LEN - 1 : msg.size());

    DWORD written = 0;
    WriteFile(hFile, buf, MAX_MSG_LEN, &written, NULL);

    hdr.writePos = (hdr.writePos + 1) % hdr.capacity;
    WriteHeader(hFile, hdr);
    CloseHandle(hFile);
    return true;
}