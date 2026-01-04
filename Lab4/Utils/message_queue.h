#pragma once
#include <string>
#include <windows.h>

constexpr int MAX_MSG_LEN = 20;

// Заголовок файла с очередью сообщений.
struct QueueHeader {
    int capacity;
    int readPos;
    int writePos;
};

// Создаёт бинарный файл с заголовком и пустыми сообщениями
bool InitMessageFile(const std::wstring& filename, int capacity);

// Читает одно сообщение из кольцевой очереди
bool ReadMessage(const std::wstring& filename, std::string& out);

// Пишет одно сообщение в кольцевую очередь
bool WriteMessage(const std::wstring& filename, const std::string& msg);
