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
    src/configHandler.cpp
)

# Include directories for headers
target_include_directories(linterLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# ----------------------------
# Library: GTest
# ----------------------------

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 6910c9d9165801d8827d628cb72eb7ea9dd538c5
  GIT_SHALLOW TRUE  # Only fetch latest commit, reduces size
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE  # Fixes the warning
  
)

FetchContent_MakeAvailable(googletest)
# Ensure gtest is linked to linterLib
target_link_libraries(linterLib PUBLIC GTest::gtest_main)

# ----------------------------
# Library: libgit2
# ----------------------------

find_package(libgit2 REQUIRED)
target_link_libraries(linterLib PUBLIC git2)

# ----------------------------
# Library: FMT
# ----------------------------

FetchContent_Declare(
        fmt
        GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
        GIT_TAG         9cf9f38eded63e5e0fb95cd536ba51be601d7fa2
        GIT_PROGRESS    TRUE
        USES_TERMINAL_DOWNLOAD TRUE
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE  # Fixes the warning
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(linterLib PUBLIC fmt::fmt)


# ----------------------------
# Library: Lohman Json API
# ----------------------------

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)


target_link_libraries(linterLib PUBLIC nlohmann_json::nlohmann_json)

# Normal executable for end users
add_executable(linter src/main.cpp)
target_link_libraries(linter PRIVATE linterLib)

# Test executable (Always Built, but Not Run Automatically)
add_executable(linterTests
    Tests/mainTest.cpp  # This file has a main() function for tests
    Tests/testArgs.cpp
    Tests/configHandlerTest.cpp
    Tests/inputTest.cpp
    Tests/scannerTest.cpp
    Tests/integrationTests.cpp
)
target_link_libraries(linterTests linterLib GTest::gtest_main)

#unzip the dummy repos

set(DUMMY_REPOS_DIR "${CMAKE_BINARY_DIR}/dummyRepos") # This is the extracted directory
set(DUMMY_ARCHIVE "${CMAKE_SOURCE_DIR}/Tests/dummyRepos.zip") # Path to your zip file

# Check if the extracted directory already exists
if(NOT EXISTS ${DUMMY_REPOS_DIR})
    message(STATUS "Extracting dummy repositories from ${DUMMY_ARCHIVE}...")

    # Ensure unzip is available
    find_program(UNZIP_EXE unzip)
    if(NOT UNZIP_EXE)
        message(FATAL_ERROR "Error: unzip command not found! Please install unzip.")
    endif()

    # Extract the dummy.zip into ${CMAKE_BINARY_DIR}
    execute_process(COMMAND ${UNZIP_EXE} -o ${DUMMY_ARCHIVE} -d ${CMAKE_SOURCE_DIR}/Tests
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Tests)
endif()


# Enable CTest and register the test
#enable_testing()
#add_test(NAME LinterTests COMMAND linterTests)