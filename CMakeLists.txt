cmake_minimum_required(VERSION 3.18)
project(logger C)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
set(CMAKE_C_STANDARD 11)

include_directories(include)
add_library(logger SHARED src/logger.c)

add_executable(LoggerTest test/main.c src/logger.c)
target_link_libraries(LoggerTest PRIVATE Threads::Threads ZLIB::ZLIB)
