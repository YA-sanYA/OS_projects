include(FetchContent)

# 1. Загружаем Google Test из интернета автоматически
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Для Windows: предотвращаем конфликт с рантаймом MSVC (MD vs MT)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Создаем исполняемый файл с тестами
add_executable(unit_tests test_queue.cpp)

# 3. Подключаем твою библиотеку utils и gtest
# gtest_main — это стандартная точка входа main() для тестов, нам её писать не нужно
target_link_libraries(unit_tests PRIVATE utils gtest_main)

# 4. Чтобы тесты появились в Обозревателе тестов Visual Studio
include(GoogleTest)
gtest_discover_tests(unit_tests)