#include <gtest/gtest.h>
#include <windows.h>
#include <thread>
#include <vector>
#include <atomic>
#include "locking.h"
#include "common.h"

TEST(LockManagerTest, MultipleReadersAllowed) {
    LockManager lm;
    int testId = 1;

    lm.lockForRead(testId);

    lm.lockForRead(testId);

    lm.unlockForRead(testId);
    lm.unlockForRead(testId);
    SUCCEED();
}

TEST(LockManagerTest, WriterBlocksReader) {
    LockManager lm;
    int testId = 10;
    std::atomic<bool> readerFinished(false);

    lm.lockForWrite(testId);

    std::thread readerThread([&]() {
        lm.lockForRead(testId);
        readerFinished = true;
        lm.unlockForRead(testId);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_FALSE(readerFinished);

    lm.unlockForWrite(testId);

    readerThread.join();
    EXPECT_TRUE(readerFinished);
}


TEST(EmployeeTest, StructSizeAndData) {
    employee emp;
    emp.num = 1;
    strcpy_s(emp.name, "Test");
    emp.hours = 40.5;

    EXPECT_EQ(emp.num, 1);
    EXPECT_STREQ(emp.name, "Test");
    EXPECT_DOUBLE_EQ(emp.hours, 40.5);

    EXPECT_LT(sizeof(emp.name), 20);
}

TEST(ServerLogicTest, FindEmployeeInVector) {
    std::vector<employee> db;
    employee e1; e1.num = 101; strcpy_s(e1.name, "Ivan");
    employee e2; e2.num = 202; strcpy_s(e2.name, "Petr");
    db.push_back(e1);
    db.push_back(e2);

    auto find_fn = [&](int id) -> employee* {
        for (auto& item : db) if (item.num == id) return &item;
        return nullptr;
        };

    EXPECT_NE(find_fn(101), nullptr);
    EXPECT_EQ(find_fn(101)->num, 101);
    EXPECT_EQ(find_fn(999), nullptr);
}