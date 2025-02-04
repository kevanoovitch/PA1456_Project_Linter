cmake_minimum_required(VERSION 3.15)

project(linter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(linterLib STATIC
    src/inputHandler.cpp
    src/app.cpp
)
find_package(GTest REQUIRED)

if(NOT GTest_FOUND)
    message(FATAL_ERROR "GTest not found!")
endif()

target_include_directories(linterLib PUBLIC include)
target_link_libraries(linterLib PUBLIC GTest::GTest GTest::Main)

# --- Switch out commented lines to switch from test and non-test exe. --- #
# add_executable(linter
# src/main.cpp
# )
# target_link_libraries(linter linterLib)
add_executable(linterTests
    Tests/inputTest.cpp
)
target_link_libraries(linterTests linterLib)
