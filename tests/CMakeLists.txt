project_log("CMake: Tests")

set(TESTS_TARGET ${LIB_TARGET}_tests)

include(CTest)

file(GLOB_RECURSE TESTS_CXX_SOURCES *.cpp)
add_executable(${TESTS_TARGET} ${TESTS_CXX_SOURCES})

target_link_libraries(${TESTS_TARGET} PUBLIC ${LIB_TARGET})
target_link_libraries(${TESTS_TARGET} PUBLIC GTest::gtest_main fmt)

include(GoogleTest)
gtest_discover_tests(${TESTS_TARGET})
