cmake_minimum_required(VERSION 3.12) 

set(PROJECT_NAME rt)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(${PROJECT_NAME})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-g")

file(COPY "res" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

file(GLOB SRC_FILES "src/*.c" "lib/mathc/*.c")
include_directories("include/")
include_directories("lib/mathc/")
include_directories("lib/stb/")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} m)
