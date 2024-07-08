cmake_minimum_required(VERSION 3.5)

project(FileTransferService LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories("${CMAKE_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CLIENT_FILES_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/client_files")
file(MAKE_DIRECTORY ${CLIENT_FILES_DIR})

add_executable(run
    run.cpp
    process/Process.h process/Process.cpp
)

add_subdirectory(server)
add_subdirectory(client)
