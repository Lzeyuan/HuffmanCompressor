cmake_minimum_required(VERSION 3.31.0)
project(huffman-compressor VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_executable(huffman-compressor
    src/main.cpp
)
target_include_directories(huffman-compressor PRIVATE
    "include"
)

add_library(huffman-compressor-lib STATIC
    src/HuffmanTree.cpp
)
target_include_directories(huffman-compressor-lib PUBLIC
    "include"
)

add_subdirectory("test")
