add_executable (tests "tests.cpp" "testUtils.cpp" "testUtils.h")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tests PROPERTY CXX_STANDARD 20)
endif()

enable_testing()


#target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)

target_link_libraries(tests PRIVATE Lib PRIVATE GTest::gtest_main PRIVATE Kernel32.lib)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/lib)

gtest_discover_tests(tests)