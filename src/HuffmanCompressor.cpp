#include "HuffmanCompressor.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "BitWriter.hpp"
#include "HuffmanTree.hpp"

namespace leza::compression::huffman::simple {
std::string Encoder::encode(const std::byte byte) {
  return codeTable_[std::to_integer<uint8_t>(byte)];
}

Decoder::Decoder(Decoder::HuffmanNodePtr &&huffmanNodePtr)
    : root_(std::move(huffmanNodePtr)) {
  current_ = root_.get();
}

bool Decoder::decode(bool bit, std::byte &outByte) {
  if (bit) {
    current_ = current_->right.get();
  } else {
    current_ = current_->left.get();
  }

  if (current_->isLeaf()) {
    outByte = static_cast<std::byte>(current_->symbol);
    current_ = root_.get();
    return true;
  }
  return false;
}

// FileCompressor::FileCompressor(const std::string &inputPath,
//                                const std::string &outputPath,
//                                CodeTable &&codeTable,
//                                HuffmanNodePtr &&huffmanTreeRoot)
//     : inputPath_(inputPath), outputPath_(outputPath), encoder_(codeTable),
//       decoder_(std::move(huffmanTreeRoot)) {}

// std::unique_ptr<FileCompressor>
// FileCompressor::create(const std::string &inputPath,
//                        const std::string &outputPath) {
//   auto frequencies = getFrequencyArray(inputPath);
//   auto [codeTable, root] = HuffmanTree::build(frequencies);
//   auto t = new FileCompressor(inputPath, outputPath, std::move(codeTable),
//                               std::move(root));
//   return std::unique_ptr<FileCompressor>{t};
// }

HuffmanTree::FrequencyArray
FileCompressor::getFrequencyArray(std::istream &inputStream,
                                  uint64_t &fileSize) {
  HuffmanTree::FrequencyArray frequencies;
  char buffer_[BUFFER_SIZE];
  fileSize = 0;
  while (inputStream.read(buffer_, BUFFER_SIZE) || inputStream.gcount() > 0) {
    size_t readCount = inputStream.gcount();
    fileSize += readCount;
    for (size_t i = 0; i < readCount; i++) {
      frequencies[i]++;
    }
  }

  return frequencies;
}

int FileCompressor::compress(const std::string &inputPath,
                             const std::string &outputPath) {
  // 打开文件
  std::ifstream inputFileStream(inputPath, std::ios::binary);
  std::ofstream outputFileStream(outputPath, std::ios::binary);

  if (!inputFileStream) {
    throw std::runtime_error("无法打开文件: " + inputPath);
  }
  if (!outputFileStream) {
    throw std::runtime_error("无法写入文件: " + outputPath);
  }
  return compress(inputFileStream, outputFileStream);
}

int FileCompressor::compress(std::istream &inputStream,
                             std::ostream &outputStream) {
  // 1.文件头
  Header header;
  header.magicNumber = MAGIC_NUMBER;
  header.version = VERSION_NUMBER;
  header.isDirectory = false;
  header.extend = 0;

  // TODO: 序列化树，

  // 2.哈夫曼树序列化数据
  

  // 1.建表
  auto frequencies = getFrequencyArray(inputStream, header.originFileSize);
  auto root = HuffmanTree::buildTree(frequencies);
  auto codeTable = HuffmanTree::buildCodeTable(root.get());
  Encoder encoder(codeTable);

  // 2.编码&写入
  inputStream.seekg(0);
  uint8_t bitCount = 0;
  uint64_t compressByteCount = 0;
  BitWriter bitWriter(outputStream);
  while (inputStream.read(buffer_, BUFFER_SIZE) || inputStream.gcount() > 0) {
    size_t readCount = inputStream.gcount();
    std::string encodeBits;
    for (size_t i = 0; i < readCount; i++) {
      encodeBits += encoder.encode(static_cast<std::byte>(buffer_[i]));
    }
    for (auto bit : encodeBits) {
      bitWriter.writeBit(bit == '0');
      bitCount++;
      if (bitCount == 8) {
        compressByteCount++;
      }
    }
  }
  if (bitCount > 8) {
    compressByteCount++;
  }

  header.compressSize = compressByteCount;
  return bitWriter.flush();
}

} // namespace leza::compression::huffman::simple

// #include <cstdint>
// #include <fstream>
// #include <string>

// #include "BitWriter.hpp"

// namespace leza::compression::huffman {
// void FileCompressor::compress() {
//   // 打开输入文件和输出文件
//   std::ifstream inputFile(inputPath_, std::ios::binary);
//   std::ofstream outputFile(outputPath_, std::ios::binary);

//   if (!inputFile.is_open()) {
//     throw std::runtime_error("无法打开输入文件！");
//   }

//   if (!outputFile.is_open()) {
//     throw std::runtime_error("无法打开输出文件！");
//   }

//   // 用于存储文件数据的缓冲区
//   char buffer[BUFFER_SIZE];

//   // 获取频率
//   while (inputFile.read(buffer, BUFFER_SIZE) || inputFile.gcount() > 0) {
//     size_t bytesRead = inputFile.gcount();
//     for (uint8_t b : buffer) {
//       frequencies_[b]++;
//     }
//   }
//   huffmanTree_.buildTree(frequencies_);

//   // 清除EOF标志和其他错误标志
//   inputFile.clear(); // 重要！必须先清除错误标志

//   // 将文件指针移回开头
//   inputFile.seekg(0, std::ios::beg);
//   BitWriter bitWriter(outputFile);
//   while (inputFile.read(buffer, BUFFER_SIZE) || inputFile.gcount() > 0) {
//     std::string t = huffmanTree_.encode(buffer, inputFile.gcount());
//     bitWriter.writeBits(t);
//   }

//   bitWriter.flush();

//   // 关闭文件
//   inputFile.close();
//   outputFile.close();
// }

// void FileCompressor::decompress(std::string_view out) {
//   // 打开输入文件和输出文件
//   std::ifstream inputFile(inputPath_, std::ios::binary);
//   std::ofstream outputFile(std::string(out), std::ios::binary);

//   if (!inputFile.is_open()) {
//     throw std::runtime_error("无法打开输入文件！");
//   }

//   if (!outputFile.is_open()) {
//     throw std::runtime_error("无法打开输出文件！");
//   }

//   // 用于存储文件数据的缓冲区
//   char buffer[BUFFER_SIZE];
//   int decodeIndex = 0;

//   std::string result;
//   // 获取频率
//   while (inputFile.read(buffer, BUFFER_SIZE) || inputFile.gcount() > 0) {
//     size_t bytesRead = inputFile.gcount();
//     huffmanTree_.decode(buffer);
//   }

//   // 关闭文件
//   inputFile.close();
//   outputFile.close();
// }
// } // namespace leza::compression::huffman