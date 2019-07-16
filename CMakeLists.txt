cmake_minimum_required(VERSION 3.3)
project(OpenMP_version)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -lcrypt")

add_executable(OpenMP_version main.cpp DictionaryDecrypter.cpp DictionaryDecrypter.h)
target_link_libraries(OpenMP_version crypt)