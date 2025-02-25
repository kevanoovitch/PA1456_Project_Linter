cmake_minimum_required(VERSION 3.15)

project(linter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

add_library(linterLib STATIC
    src/inputHandler.cpp
    src/main.cpp
    src/fileManager.cpp
    src/scanner.cpp
    src/resultInterpreter.cpp
    src/constants.cpp
    Tests/testArgs.cpp
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



target_include_directories(linterLib PUBLIC ${LIBGIT2_INCLUDE_DIRS})
target_link_libraries(linterLib PUBLIC ${LIBGIT2_LIBRARIES})

#FMT
#FetchContent_Declare(
#        fmt
#        GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
        #GIT_TAG         9cf9f38eded63e5e0fb95cd536ba51be601d7fa2
        #GIT_PROGRESS    TRUE
        #USES_TERMINAL_DOWNLOAD TRUE
#)
#FetchContent_MakeAvailable(fmt)
#target_link_directories(linterLib PUBLIC fmt::fmt)

#FMT bad manual solution
set(FMT_INCLUDE_DIR "/home/kevin/libs/fmt/include")
set(FMT_LIBRARY "/home/kevin/libs/fmt/build/libfmt.a") 

target_include_directories(linterLib PUBLIC ${FMT_INCLUDE_DIR})
target_link_libraries(linterLib PUBLIC ${FMT_LIBRARY})


#Lohman Json API

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE  # Fixes the warning
)
FetchContent_MakeAvailable(json)

target_link_libraries(linterLib PRIVATE nlohmann_json::nlohmann_json)

# Define a manual toggle for testing mode
set(ENABLE_TESTS ON)  # Change to ON if you want to build the test executable

if(ENABLE_TESTS)
    message(STATUS "Building test executable")
    add_executable(linterTests 
        Tests/testArgs.cpp 
        Tests/mainTest.cpp 
        Tests/inputTest.cpp 
        Tests/scannerTest.cpp
        Tests/systemTest.cpp
        )
    target_link_libraries(linterTests linterLib)
else()
    message(STATUS "Building normal executable")
    add_executable(linter src/main.cpp)
    target_link_libraries(linter linterLib)
endif()