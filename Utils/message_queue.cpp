#include "message_queue.h"
#include <vector>

// Вычисляет смещение в файле, где лежит сообщение с индексом index.
static long long GetMessageOffset(int index) {
    return sizeof(QueueHeader) + index * MAX_MSG_LEN;
}

/// Читает заголовок очереди
static bool ReadHeader(HANDLE hFile, QueueHeader& hdr) {
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN); // переход к началу файла
    DWORD read = 0;
    return ReadFile(hFile, &hdr, sizeof(hdr), &read, NULL) && read == sizeof(hdr);
}

// Записывает заголовок очереди назад в начало файла
static bool WriteHeader(HANDLE hFile, const QueueHeader& hdr) {
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD written = 0;
    return WriteFile(hFile, &hdr, sizeof(hdr), &written, NULL) && written == sizeof(hdr);
}

// Инициализирует новый бинарный файл очереди.
bool InitMessageFile(const std::wstring& filename, int capacity) {

    HANDLE hFile = CreateFileW(
        filename.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // 1. Формируем заголовок
    QueueHeader hdr{};
    hdr.capacity = capacity;
    hdr.readPos = 0;
    hdr.writePos = 0;

    // 2. Записываем заголовок в начало файла
    DWORD written = 0;
    WriteFile(hFile, &hdr, sizeof(hdr), &written, NULL);

    // 3. Записываем пустые сообщения (все нули)
    std::vector<char> empty(MAX_MSG_LEN, 0);

    for (int i = 0; i < capacity; i++) {
        WriteFile(hFile, empty.data(), MAX_MSG_LEN, &written, NULL);
    }

    CloseHandle(hFile);
    return true;
}

// Читает сообщение в формате FIFO.
bool ReadMessage(const std::wstring& filename, std::string& out) {

    HANDLE hFile = CreateFileW(
        filename.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // 1. Читаем заголовок
    QueueHeader hdr{};
    if (!ReadHeader(hFile, hdr)) {
        CloseHandle(hFile);
        return false;
    }

    // 2. Вычисляем offset для чтения
    long long offset = GetMessageOffset(hdr.readPos);
    SetFilePointer(hFile, offset, NULL, FILE_BEGIN);

    // 3. Читаем сообщение фиксированного размера
    char buf[MAX_MSG_LEN + 1] = {};
    DWORD read = 0;
    ReadFile(hFile, buf, MAX_MSG_LEN, &read, NULL);

    out = buf;

    // 4. Сдвигаем указатель чтения
    hdr.readPos = (hdr.readPos + 1) % hdr.capacity;

    // 5. Записываем обновлённый заголовок
    WriteHeader(hFile, hdr);

    CloseHandle(hFile);
    return true;
}

// Пишет сообщение в кольцевую очередь.
bool WriteMessage(const std::wstring& filename, const std::string& msg) {

    HANDLE hFile = CreateFileW(
        filename.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // 1. Читаем заголовок
    QueueHeader hdr{};
    if (!ReadHeader(hFile, hdr)) {
        CloseHandle(hFile);
        return false;
    }

    // 2. Позиция записи
    long long offset = GetMessageOffset(hdr.writePos);
    SetFilePointer(hFile, offset, NULL, FILE_BEGIN);

    // 3. Подготавливаем буфер фиксированного размера
    char buf[MAX_MSG_LEN] = {};
    memcpy(buf, msg.c_str(), min((int)msg.size(), MAX_MSG_LEN));

    // 4. Записываем сообщение
    DWORD written = 0;
    WriteFile(hFile, buf, MAX_MSG_LEN, &written, NULL);

    // 5. Сдвигаем writePos
    hdr.writePos = (hdr.writePos + 1) % hdr.capacity;

    // 6. Сохраняем обновлённый заголовок
    WriteHeader(hFile, hdr);

    CloseHandle(hFile);
    return true;
}
