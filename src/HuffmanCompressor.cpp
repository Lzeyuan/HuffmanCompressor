#include "HuffmanCompressor.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "BitWriter.hpp"
#include "HuffmanTree.hpp"

namespace leza::compression::huffman::classic {
// std::string Encoder::encode(const std::byte byte) {
//   return codeTable_[std::to_integer<uint8_t>(byte)];
// }

// Decoder::Decoder(Decoder::HuffmanNodePtr &&huffmanNodePtr)
//     : root_(std::move(huffmanNodePtr)) {
//   current_ = root_.get();
// }

// bool Decoder::decode(bool bit, std::byte &outByte) {
//   if (bit) {
//     current_ = current_->right.get();
//   } else {
//     current_ = current_->left.get();
//   }

//   if (current_->isLeaf()) {
//     outByte = static_cast<std::byte>(current_->symbol);
//     current_ = root_.get();
//     return true;
//   }
//   return false;
// }

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

auto FileCompressor::getFrequencyArray(std::istream &inputStream,
                                       uint64_t &fileSize) -> FrequencyArray {
  FrequencyArray frequencies;
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

  // 2.哈夫曼树压缩&写入
  auto frequencies = getFrequencyArray(inputStream, header.originFileSize);
  int compressByteCount =
      ClassicHuffmanTreeEncoder::encode(inputStream, outputStream, frequencies);
  header.compressSize = compressByteCount;
  return 0;
}

void FileCompressor::write4Byte(std::ostream &os, uint32_t v) {
  Byte b[4] = {Byte((v >> 24) & 0xFF), Byte((v >> 16) & 0xFF),
               Byte((v >> 8) & 0xFF), Byte(v & 0xFF)};
  os.write(reinterpret_cast<char *>(b), 4);
}

void FileCompressor::buildSerializationArrays(const HuffmanNode *node,
                                              std::vector<bool> &structBits,
                                              std::vector<Byte> &leaves) {
  if (!node) {
    return;
  }

  if (node->isLeaf()) {
    structBits.push_back(true);     // 1 = leaf
    leaves.push_back(node->symbol); // 叶子数据按先序收集
  } else {
    structBits.push_back(false); // 0 = internal
    buildSerializationArrays(node->left.get(), structBits, leaves);
    buildSerializationArrays(node->right.get(), structBits, leaves);
  }
}

void FileCompressor::bits2BytesWithMSBF(const std::vector<bool> &bits,
                                        std::vector<Byte> &out) {
  size_t bitCount = bits.size();
  size_t byteCount = (bitCount) / 8;
  out.assign(byteCount, 0);
  for (size_t i = 0; i < byteCount; ++i) {
    int bitIndex = i << 3;
    for (size_t j = 0; j < 8; j++) {
      if (bits[bitIndex + j]) {
        out[i] |= 1u << j;
      }
    }
  }
}

void FileCompressor::serializeTree2Stream(const HuffmanNode *root,
                                          std::ostream &os) {
  std::vector<bool> structBits;
  std::vector<Byte> leaves;
  buildSerializationArrays(root, structBits, leaves);

  // 1.写入树结构序列化长度
  uint32_t structBitsLen = uint32_t(structBits.size());
  write4Byte(os, structBitsLen);

  // 2.写入子节点列表序列化长度
  uint32_t leafCount = uint32_t(leaves.size());
  write4Byte(os, leafCount);

  // 3. 写入树结构
  std::vector<Byte> packed;
  bits2BytesWithMSBF(structBits, packed);
  if (!packed.empty()) {
    os.write(reinterpret_cast<char *>(packed.data()), packed.size());
  }

  // 4. 写入节点列表
  if (!leaves.empty()) {
    os.write(reinterpret_cast<char *>(leaves.data()), leaves.size());
  }
}

int ClassicHuffmanTreeEncoder::encode(std::istream &inputStream,
                                      std::ostream &outputStream,
                                      const FrequencyArray &frequencyArray) {
  auto root = buildTree(frequencyArray);
  auto huffmanCollector = collectTreeByPreorder(root.get());

  uint64_t compressByteCount = 0;
  compressByteCount += serialize(outputStream, root.get());

  uint8_t bitCount = 0;
  BitWriter bitWriter(outputStream);
  char buffer[BUFFER_SIZE];

  while (inputStream.read(buffer, BUFFER_SIZE) || inputStream.gcount() > 0) {
    size_t readCount = inputStream.gcount();
    std::string encodeBits;
    // 编码
    for (size_t i = 0; i < readCount; i++) {
      encodeBits += huffmanCollector.codeTable[buffer[i]];
    }
    // 写入
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
  return compressByteCount;
}

int ClassicHuffmanTreeEncoder::serialize(std::ostream &os,
                                         const HuffmanNode *root) {
  // 1. 获取树结构、子节点列表、
  std::vector<bool> structBits;
  std::vector<uint8_t> leaves;
  buildSerializationArrays(root, structBits, leaves);

  // 1.写入树结构序列化长度
  uint32_t structBitsLen = uint32_t(structBits.size());
  write4Byte(os, structBitsLen);

  // 2.写入子节点列表序列化长度
  uint32_t leafCount = uint32_t(leaves.size());
  write4Byte(os, leafCount);

  // 3. 写入树结构
  std::vector<Byte> packed;
  bits2BytesWithMSBF(structBits, packed);
  if (!packed.empty()) {
    os.write(reinterpret_cast<char *>(packed.data()), packed.size());
  }

  // 4. 写入节点列表
  if (!leaves.empty()) {
    os.write(reinterpret_cast<char *>(leaves.data()), leaves.size());
  }
}

ClassicHuffmanTreeEncoder::HuffmanNodePtr
ClassicHuffmanTreeEncoder::buildTree(const FrequencyArray &frequencyArray) {
  using FrequencyNode = std::pair<uint64_t, HuffmanNodePtr>;

  auto huffmanNodeCmp = [](const FrequencyNode &a,
                           const FrequencyNode &b) -> bool {
    return a.first > b.first;
  };

  std::priority_queue<FrequencyNode, std::vector<FrequencyNode>,
                      decltype(huffmanNodeCmp)>
      pq;

  for (int i = 0; i < 256; ++i) {
    if (frequencyArray[i] > 0) {
      pq.push({frequencyArray[i], std::make_unique<HuffmanNode>(i)});
    }
  }

  if (pq.size() == 0) {
    return nullptr;
  }

  // 单字符文件兜底
  if (pq.size() == 1) {
    uint8_t dummyChar = frequencyArray[0] > 0 ? 1 : 0;
    pq.push({0, std::make_unique<HuffmanNode>(dummyChar)});
  }

  while (pq.size() > 1) {
    auto [freq1, left] = std::move(const_cast<FrequencyNode &>(pq.top()));
    pq.pop();

    auto [freq2, right] = std::move(const_cast<FrequencyNode &>(pq.top()));
    pq.pop();

    auto new_node =
        std::make_unique<HuffmanNode>(std::move(left), std::move(right));
    pq.push({freq1 + freq2, std::move(new_node)});
  }

  auto [_, root] = std::move(const_cast<FrequencyNode &>(pq.top()));
  return std::move(root);
}

// 注意：目前使用前序遍历哈夫曼树
ClassicHuffmanTreeEncoder::HuffmanCollector
ClassicHuffmanTreeEncoder::collectTreeByPreorder(const HuffmanNode *root) {
  HuffmanCollector huffmanCollector;
  auto generate = [](this auto &&generate, const HuffmanNode *node,
                     const std::string &code,
                     HuffmanCollector &huffmanCollector) -> void {
    if (!node) {
      return;
    }
    huffmanCollector.onEnter(node, code);
    if (node->left) {
      generate(node->left.get(), code + "0", huffmanCollector);
    }
    if (node->right) {
      generate(node->right.get(), code + "1", huffmanCollector);
    }
  };
  generate(root, "", huffmanCollector);
  return huffmanCollector;
}

// 注意：目前使用前序遍历哈夫曼树
// 如果需要修改中序遍历和后序遍历，修改该函数在ClassicHuffmanTreeEncoder::buildCodeTable中的顺序即可。
// 一次遍历获取：树结构、子节点列表、码表
// 树结构、子节点列表：用于序列化哈夫曼树，码表：用于压缩
// 0节点，1叶子
void ClassicHuffmanTreeEncoder::HuffmanCollector::onEnter(
    const HuffmanNode *node, const std::string &code) {
  if (node->isLeaf()) {
    structBits.push_back(true);
    leaves.push_back(node->symbol);
  } else {
    structBits.push_back(false);
  }
}

} // namespace leza::compression::huffman::classic