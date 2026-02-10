#include <gtest/gtest.h>

#include <array>
#include <print>

#include "HuffmanTree.hpp"

TEST(Huffman, PrintTree) {
  using leza::compression::huffman::HuffmanTree;
  constexpr int BYTE_SIZE = 256;
  std::vector<uint8_t> input;
  for (int i = 0; i < 8; i++) {
    int power = 1 << i;
    for (int j = 0; j < power; j++) {
      input.push_back(i);
    }
  }

  std::array<uint64_t, 256> frequencies{};

  for (auto byte : input) {
    frequencies[byte]++;
  }

  auto root = HuffmanTree::buildTree(frequencies);
  auto codeTable = HuffmanTree::buildCodeTable(root.get());

  HuffmanTree::printTree(root.get());
  for (int i = 0; i < BYTE_SIZE; i++) {
    if (!codeTable[i].empty()) {
      std::println("{}, {}", i, codeTable[i]);
    }
  }
}

TEST(Huffman, SingleByte) {
  using leza::compression::huffman::HuffmanTree;
  constexpr int BYTE_SIZE = 256;
  std::vector<uint8_t> input;
  for (int i = 0; i < 8; i++) {
    input.push_back(1);
  }

  std::array<uint64_t, 256> frequencies{};

  for (auto byte : input) {
    frequencies[byte]++;
  }

  auto root = HuffmanTree::buildTree(frequencies);
  auto codeTable = HuffmanTree::buildCodeTable(root.get());

  ASSERT_TRUE(codeTable[0] != "");
  ASSERT_TRUE(codeTable[1] != "");
  ASSERT_TRUE(codeTable[2] == "");
}

TEST(Huffman, SingleByteZero) {
  using leza::compression::huffman::HuffmanTree;
  constexpr int BYTE_SIZE = 256;
  std::vector<uint8_t> input;
  for (int i = 0; i < 8; i++) {
    input.push_back(0);
  }

  std::array<uint64_t, 256> frequencies{};

  for (auto byte : input) {
    frequencies[byte]++;
  }

  auto root = HuffmanTree::buildTree(frequencies);
  auto codeTable = HuffmanTree::buildCodeTable(root.get());

  ASSERT_TRUE(codeTable[0] != "");
  ASSERT_TRUE(codeTable[1] != "");
  ASSERT_TRUE(codeTable[2] == "");
}

TEST(Huffman, Empty) {
  using leza::compression::huffman::HuffmanTree;
  constexpr int BYTE_SIZE = 256;
  std::vector<uint8_t> input;

  std::array<uint64_t, 256> frequencies{};

  for (auto byte : input) {
    frequencies[byte]++;
  }

  auto root = HuffmanTree::buildTree(frequencies);
  auto codeTable = HuffmanTree::buildCodeTable(root.get());

  ASSERT_TRUE(root == nullptr);
}