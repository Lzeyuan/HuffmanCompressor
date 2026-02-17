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

// 4 magicNumber
// 1 version
// 8 originSize
// 4 serializeTreeSize*
// 4 compressSize*
// 1 compressPadding*
// 1 type
// 4 extend
// *是哈夫曼编码后在知道
int FileCompressor::compress(std::istream &inputStream,
                             std::ostream &outputStream) {
  // 1.文件头
  Header header;
  header.magicNumber = MAGIC_NUMBER;
  header.version = VERSION_NUMBER;
  header.isDirectory = false;
  header.extend = 0;

  inputStream.seekg(0, std::ios::end);
  header.originFileSize = inputStream.tellg();
  inputStream.seekg(0, std::ios::beg);

  // 写入文件头，树结构大小、压缩大小待定
  wirteHeader(outputStream, header);

  // 2.哈夫曼树压缩&写入
  auto encodeResult =
      ClassicHuffmanTreeEncoder::encode(inputStream, outputStream);
  header.compressSize = encodeResult.compressSize;

  // 补充文件头信息
  outputStream.seekp(14, std::ios::beg);
  writeInBigEnd(outputStream, encodeResult.serializeTreeSize);
  writeInBigEnd(outputStream, encodeResult.compressSize);
  writeInBigEnd(outputStream, encodeResult.compressPadding);

  // TODO: crc32校验

  return 0;
}

void FileCompressor::wirteHeader(std::ostream &outputStream, Header header) {
  writeInBigEnd(outputStream, header.magicNumber);
  writeInBigEnd(outputStream, header.version);
  writeInBigEnd(outputStream, header.originFileSize);
  writeInBigEnd(outputStream, header.huffmanTreeSize);
  writeInBigEnd(outputStream, header.compressSize);
  writeInBigEnd(outputStream, header.isDirectory);
  writeInBigEnd(outputStream, header.extend);
}

ClassicHuffmanTreeEncoder::EncodeResult
ClassicHuffmanTreeEncoder::encode(std::istream &inputStream,
                                  std::ostream &outputStream) {
  FrequencyArray frequencies = getFrequencyArray(inputStream);
  auto root = buildTree(frequencies);
  auto huffmanCollector = collectTreeByPreorder(root.get());

  // 1.压缩头，树信息
  uint32_t compressByteCount = 0;
  uint32_t serializeTreeSize = serialize(
      outputStream, huffmanCollector.structBits, huffmanCollector.leaves);
  compressByteCount += serializeTreeSize;

  // 2.压缩文件
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
  return {compressByteCount, serializeTreeSize,
          static_cast<uint8_t>(8 - bitCount)};
}

ClassicHuffmanTreeEncoder::FrequencyArray
ClassicHuffmanTreeEncoder::getFrequencyArray(std::istream &inputStream) {
  char buffer_[BUFFER_SIZE];
  FrequencyArray frequencies;
  while (inputStream.read(buffer_, BUFFER_SIZE) || inputStream.gcount() > 0) {
    size_t readCount = inputStream.gcount();
    for (size_t i = 0; i < readCount; i++) {
      frequencies[i]++;
    }
  }
  return frequencies;
}

int ClassicHuffmanTreeEncoder::serialize(std::ostream &os,
                                         const std::vector<bool> structBits,
                                         const std::vector<uint8_t> leaves) {
  BitWriter bitWriter(os);
  uint16_t structsLen = (structBits.size() + 7) / 8;
  uint16_t leavesLen = leaves.size();

  auto writeUint16 = [&os](uint16_t data) {
    os.put(static_cast<char>(data >> 8));
    os.put(static_cast<char>(data & 0xFF));
  };

  // 树
  writeUint16(structsLen);
  for (auto b : structBits) {
    bitWriter.writeBit(b);
  }
  bitWriter.flush();

  // 叶子
  writeUint16(leavesLen);
  os.write(reinterpret_cast<const char *>(leaves.data()), leaves.size());
  return 2 + 2 + structsLen + leavesLen;
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
    generate(node->left.get(), code + "0", huffmanCollector);
    generate(node->right.get(), code + "1", huffmanCollector);
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