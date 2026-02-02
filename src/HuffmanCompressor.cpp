#include "HuffmanCompressor.hpp"
#include <iostream>
#include <string>

namespace leza::compression::huffman {
void FileCompressor::compress() {
  // 打开输入文件和输出文件
  std::ifstream inputFile(inputPath_, std::ios::binary);
  std::ofstream outputFile(outputPath_, std::ios::binary);

  if (!inputFile.is_open()) {
    throw std::runtime_error("无法打开输入文件！");
  }

  if (!outputFile.is_open()) {
    throw std::runtime_error("无法打开输出文件！");
  }

  // 用于存储文件数据的缓冲区
  char buffer[BUFFER_SIZE];

  // 获取频率
  while (inputFile.read(buffer, BUFFER_SIZE) || inputFile.gcount() > 0) {
    size_t bytesRead = inputFile.gcount();
    for (auto b : buffer) {
      frequencies_[b]++;
    }
  }
  huffmanTree_.buildTree(frequencies_);

  inputFile.seekg(0);
  BitWriter bitWriter(outputFile);
  while (inputFile.read(buffer, BUFFER_SIZE) || inputFile.gcount() > 0) {
    std::string t = huffmanTree_.encode(buffer, inputFile.gcount());
    bitWriter.writeBits(t);
  }

  bitWriter.flush();

  // 关闭文件
  inputFile.close();
  outputFile.close();
}
} // namespace leza::compression::huffman