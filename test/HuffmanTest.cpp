#include <gtest/gtest.h>

#include <array>
#include <print>

#include "HuffmanTree.hpp"

TEST(Huffman, SingleSymbol) {
  using leza::compression::huffman::HuffmanTree;
  std::vector<uint8_t> input = {'A', 'A', 'A', 'A', 'B', 'C', 'C', 'F'};

  HuffmanTree tree;
  std::array<uint64_t, 256> frequencies;

  for (auto byte : input) {
    frequencies[byte]++;
  }

  tree.buildTree(frequencies);
  tree.printTree();
  tree.printTable();
  std::println("{}", tree.encode(input));
}