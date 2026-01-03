#include <gtest/gtest.h>
#include <windows.h>
#include <string>
#include <vector>
#include "message_queue.h"

class MessageQueueTest : public ::testing::Test {
protected:
    std::wstring testFileName = L"test_message_queue.bin";

    void SetUp() override {
        DeleteFileW(testFileName.c_str());
    }

    void TearDown() override {
        DeleteFileW(testFileName.c_str());
    }
};

// Тест 1: Проверка создания файла
TEST_F(MessageQueueTest, CanInitializeFile) {
    bool result = InitMessageFile(testFileName, 10);
    EXPECT_TRUE(result);

    DWORD attr = GetFileAttributesW(testFileName.c_str());
    EXPECT_NE(attr, INVALID_FILE_ATTRIBUTES);
}

// Тест 2: Проверка записи и чтения (FIFO)
TEST_F(MessageQueueTest, HandlesReadWriteFIFO) {
    InitMessageFile(testFileName, 5);

    std::string msg1 = "Hello";
    std::string msg2 = "World";

    EXPECT_TRUE(WriteMessage(testFileName, msg1));
    EXPECT_TRUE(WriteMessage(testFileName, msg2));

    std::string out1, out2;
    EXPECT_TRUE(ReadMessage(testFileName, out1));
    EXPECT_TRUE(ReadMessage(testFileName, out2));

    EXPECT_EQ(out1, msg1);
    EXPECT_EQ(out2, msg2);
}

// Тест 3: Проверка циклической перезаписи (Кольцо)
TEST_F(MessageQueueTest, CircularBufferLogic) {
    int capacity = 2;
    InitMessageFile(testFileName, capacity);

    WriteMessage(testFileName, "Msg1");
    WriteMessage(testFileName, "Msg2");

    std::string out;
    ReadMessage(testFileName, out);
    EXPECT_EQ(out, "Msg1");

    EXPECT_TRUE(WriteMessage(testFileName, "Msg3"));

    ReadMessage(testFileName, out);
    EXPECT_EQ(out, "Msg2");
    ReadMessage(testFileName, out);
    EXPECT_EQ(out, "Msg3");
}

// Тест 4: Проверка ограничения длины сообщения
TEST_F(MessageQueueTest, RespectsMaxMessageLength) {
    InitMessageFile(testFileName, 1);

    std::string longMsg = "This message is definitely longer than twenty characters";
    WriteMessage(testFileName, longMsg);

    std::string out;
    ReadMessage(testFileName, out);

    EXPECT_LE(out.length(), 20);
}