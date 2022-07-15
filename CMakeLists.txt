cmake_minimum_required(VERSION 3.18)
project(logger C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_library(logger SHARED src/logger.c)

add_executable(LoggerTest test/main.c src/logger.c)
link_libraries(pthread)
