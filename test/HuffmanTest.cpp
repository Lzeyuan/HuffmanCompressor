#include <gtest/gtest.h>

#include <array>
#include <print>

#include "HuffmanTree.hpp"

TEST(Huffman, PrintTree) {
  using leza::compression::huffman::HuffmanTree;
  constexpr int BYTE_SIZE = 256;
  std::vector<uint8_t> input{'A', 'A', 'A', 'A', 'B', 'C', 'C', 'F'};

  std::array<uint64_t, 256> frequencies{};

  for (auto byte : input) {
    frequencies[byte]++;
  }

  auto [codeTable, root] = HuffmanTree::build(frequencies);

  HuffmanTree::printTree(root.get());
  for (int i = 0; i < BYTE_SIZE; i++) {
    if (!codeTable[i].empty()) {
      std::println("{:c}, {}", i, codeTable[i]);
    }
  }
}