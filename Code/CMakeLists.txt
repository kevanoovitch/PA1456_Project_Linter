cmake_minimum_required(VERSION 3.15)

project(linter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(linterLib STATIC
    src/inputHandler.cpp
    src/main.cpp
    src/fileManager.cpp
    src/scanner.cpp
    src/resultInterpreter.cpp
    src/constants.cpp
)
find_package(GTest REQUIRED)

if(NOT GTest_FOUND)
    message(FATAL_ERROR "GTest not found!")
endif()

target_include_directories(linterLib PUBLIC include)
target_link_libraries(linterLib PUBLIC GTest::GTest GTest::Main)

# libgit2
find_package(libgit2 REQUIRED) # finds the package based on standard location(?)/*

set(LIBGIT2_INCLUDE_DIRS "/home/kevin/.local/include")
set(LIBGIT2_LIBRARIES "/home/kevin/.local/lib/libgit2.so")

#message(STATUS "libgit2 include dirs: ${LIBGIT2_INCLUDE_DIRS}")
#message(STATUS "libgit2 libraries: ${LIBGIT2_LIBRARIES}")


target_include_directories(linterLib PUBLIC ${LIBGIT2_INCLUDE_DIRS})
target_link_libraries(linterLib PUBLIC ${LIBGIT2_LIBRARIES})

# Define a manual toggle for testing mode
set(ENABLE_TESTS OFF)  # Change to ON if you want to build the test executable

if(ENABLE_TESTS)
    message(STATUS "Building test executable")
    add_executable(linterTests 
        Tests/testArgs.cpp 
        Tests/mainTest.cpp 
        Tests/inputTest.cpp 
        Tests/scannerTest.cpp)
    target_link_libraries(linterTests linterLib)
else()
    message(STATUS "Building normal executable")
    add_executable(linter src/main.cpp)
    target_link_libraries(linter linterLib)
endif()