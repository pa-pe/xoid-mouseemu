cmake_minimum_required(VERSION 3.10)
project(xoid_mouseemu C)

set(CMAKE_C_STANDARD 99)
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB SOURCES "${SOURCE_DIR}/*.c")
file(GLOB HEADERS "${SOURCE_DIR}/*.h")

add_executable(xoid_mouseemu ${SOURCES})
target_include_directories(xoid_mouseemu PRIVATE ${SOURCE_DIR})
