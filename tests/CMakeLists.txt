cmake_minimum_required(VERSION 3.10)
project(TextExpansionKeyboardTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library (platform-independent logic)
add_library(core_lib STATIC
    ${CMAKE_SOURCE_DIR}/../src/InputBuffer.cpp
    ${CMAKE_SOURCE_DIR}/../src/ExpansionDictionary.cpp
)

target_include_directories(core_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/../src
)

# Test executable
add_executable(run_tests test_main.cpp)
target_link_libraries(run_tests core_lib)
