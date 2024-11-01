# Add source files in the src directory
file(GLOB_RECURSE SRC_SOURCES "*.c")
file(GLOB_RECURSE SRC_HEADERS "*.h")
file(GLOB_RECURSE TEST_HEADERS "*.hh")
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_library(ifj_lib ${SRC_SOURCES} ${SRC_HEADERS})
add_library(ifj_test ${TEST_SOURCES} ${TEST_HEADERS})

target_include_directories(ifj_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "SRC_SOURCES: ${SRC_SOURCES}")
message(STATUS "SRC_HEADERS: ${SRC_HEADERS}")
message(STATUS "TEST_SOURCES: ${TEST_SOURCES}")
message(STATUS "TEST_HEADERS: ${TEST_HEADERS}")

enable_testing()
include(GoogleTest)

add_executable(IFJapp main.c)
target_link_libraries(IFJapp ifj_lib)


add_executable(IFJtest tests.cpp)
target_link_libraries(IFJtest ifj_test gtest_main ifj_lib)
gtest_discover_tests(IFJtest)