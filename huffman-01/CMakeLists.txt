cmake_minimum_required(VERSION 3.31.0)

project(huffman-01 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_executable(huffman-01-compressor
    src/Main.cpp
)

add_library(
    HuffmanCompressorLib STATIC
    src/HuffmanCompressor.cpp
    src/BitWriter.cpp
)

target_include_directories(HuffmanCompressorLib PUBLIC
    "src"
)

add_subdirectory("test")
