#include "testUtils.h"

// Базовый тест: 3 потока, массив 10
TEST(MarkerThreads, BasicFlow) {
    EXPECT_NO_THROW(RunMarkerThreadsTest(10, 3));
}

// Граничный случай: 1 поток (минимум)
TEST(MarkerThreads, SingleThread) {
    EXPECT_NO_THROW(RunMarkerThreadsTest(5, 1));
}

// Стресс-тест: Мало места, много потоков (насыщение)
// Здесь потоки будут постоянно конфликтовать за ячейки
TEST(MarkerThreads, HighContention) {
    EXPECT_NO_THROW(RunMarkerThreadsTest(2, 10));
}

// Максимально допустимое кол-во потоков для WinAPI (64)
TEST(MarkerThreads, MaxWinApiThreads) {
    EXPECT_NO_THROW(RunMarkerThreadsTest(100, 64));
}

// Проверка валидации входных данных в утилитах
TEST(UtilsTest, NaturalNumberValidation) {
    EXPECT_ANY_THROW(readNaturalNumber("msg", "err"));
}