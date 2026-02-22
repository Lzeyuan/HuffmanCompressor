#include <cstdint>
#include <gtest/gtest.h>
#include <print>
#include <span>
#include <vector>

#include "HuffmanCompressor.hpp"

TEST(HuffmanCompressorTest, getFrequencyArrayTest) {
  using namespace leza::compression::huffman::classic::core;
  constexpr size_t BYTE_SIZE = 256;

  std::vector<uint8_t> data;
  for (int i = 0; i < BYTE_SIZE; i++) {
    for (int j = 0; j <= i; j++) {
      data.emplace_back(i);
    }
  }
  auto t = leza::compression::huffman::classic::core::getFrequencyArray(data);
  for (int i = 0; i < BYTE_SIZE; i++) {
    std::print("{}: {} ", i, t[i]);
    if (i % 10 == 0) {
      std::println();
    }
  }
  std::println();
}

// 可以层序遍历测试
TEST(HuffmanCompressorTest, buildTreeTest) {
  using namespace leza::compression::huffman::classic::core;
  FrequencyArray frequencyArray{};
  frequencyArray[0] = 1;
  frequencyArray[1] = 2;
  frequencyArray[2] = 4;
  frequencyArray[3] = 8;

  auto root = buildTree(frequencyArray);

  auto printHuffmanTree = [](this auto &&printHuffmanTree,
                             const HuffmanNode *const node,
                             const std::string &prefix, bool is_left) {
    if (!node) {
      return;
    }

    std::print("{}{}", prefix, is_left ? "├── " : "└── ");

    if (node->isLeaf()) {
      std::println("({})", static_cast<uint8_t>(node->symbol));
    } else {
      std::println("[{}]", is_left ? "0" : "1");
    }

    const std::string child_prefix = prefix + (is_left ? "│   " : "    ");

    if (node->left)
      printHuffmanTree(node->left.get(), child_prefix, true);
    if (node->right)
      printHuffmanTree(node->right.get(), child_prefix, false);
  };
  printHuffmanTree(root.get(), "", false);
}

TEST(HuffmanCompressorTest, collectTreeByPreorderTest) {
  using namespace leza::compression::huffman::classic::core;
  FrequencyArray frequencyArray{};
  frequencyArray[0] = 1;
  frequencyArray[1] = 2;
  frequencyArray[2] = 4;
  frequencyArray[3] = 8;

  auto root = buildTree(frequencyArray);

  auto res = collectTreeByPreorder(root.get());
  std::println("{}", res.leaves);
  for (int i = 0; i < res.codeTable.size(); i++) {
    if (!res.codeTable[i].empty()) {
      std::print("{}: {}, ", i, res.codeTable[i]);
    }
  }
  std::println();
  for (int i = 0; i < res.structBits.size(); i++) {
    std::print("{}", res.structBits[i]);
  }
  std::println();
}

TEST(HuffmanCompressorTest, encodeTest) {
  using namespace leza::compression::huffman::classic;
  std::vector<uint8_t> inputData;
  for (int i = 0; i < 4; i++) {
    for (int j = 1 << i; j > 0; j--) {
      inputData.emplace_back(i);
    }
  }

  auto res = encode(inputData);
  auto outputData = std::span(res.data);
  std::println("{} , {}", res.compressSize, res.data.size());

  int idx = 0;
  uint16_t leavesLen = (static_cast<uint16_t>(outputData[idx]) << 8) |
                       (static_cast<uint16_t>(outputData[idx + 1]));
  std::println("leavesLen = {}", leavesLen);

  idx += 2;
  for (int i = idx; i < idx + leavesLen; i++) {
    std::print("{:d} ", outputData[i]);
  }
  idx += leavesLen;
  std::println();

  uint16_t structsLen = (static_cast<uint16_t>(outputData[idx]) << 8) |
                        (static_cast<uint16_t>(outputData[idx + 1]));
  std::println("structsLen = {}", structsLen);

  idx += 2;
  for (int i = idx; i < idx + structsLen; i++) {
    std::print("{}", outputData[i]);
  }
  idx += structsLen;
  std::println();

  std::println("encode fileData:");
  auto fileData = outputData.subspan(idx);
  for (auto d : fileData) {
    std::print("{} ", d);
  }
  std::println();
}

TEST(HuffmanCompressorTest, deserializeTest) {
  using namespace leza::compression::huffman::classic;
  std::vector<uint8_t> inputData;
  for (int i = 0; i < 8; i++) {
    for (int j = 1 << i; j > 0; j--) {
      inputData.emplace_back(i);
    }
  }

  auto encodeResult = encode(inputData);

  auto outputData = encodeResult.data;
  auto deSerializeResult = core::deserialize(outputData);
  EXPECT_EQ(deSerializeResult.readCount, 2 + 8 + 2 + 15);

  auto decodeResult = decode(outputData);

  EXPECT_EQ(inputData, decodeResult.data);
}