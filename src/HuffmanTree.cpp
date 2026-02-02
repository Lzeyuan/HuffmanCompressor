#include "HuffmanTree.hpp"

#include <cstddef>
#include <memory>
#include <print>
#include <queue>
#include <string>

namespace leza::compression::huffman {
void HuffmanTree::buildTree(const uint8_t *bytes, size_t size) noexcept {
  FrequencyArray frequencies;
  for (int i = 0; i < size; i++) {
    frequencies[bytes[i]]++;
  }
  this->buildTree(frequencies);
}

void HuffmanTree::buildTree(const FrequencyArray &freq) noexcept {
  using std::priority_queue;
  using std::unique_ptr;
  using std::vector;
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

  auto huffmanNodeCmp = [](const HuffmanNode *a, const HuffmanNode *b) -> bool {
    return a->freq > b->freq;
  };

  priority_queue<HuffmanNode *, vector<HuffmanNode *>, decltype(huffmanNodeCmp)>
      pq;

  for (int i = 0; i < 256; ++i) {
    if (freq[i] > 0) {
      pq.push(new HuffmanNode(i, freq[i]));
    }
  }

  // 单字符文件兜底
  if (pq.size() == 1) {
    uint8_t dummyChar = freq[0] > 0 ? 1 : 0;
    pq.push(new HuffmanNode(dummyChar, 0));
  }

  while (pq.size() > 1) {
    HuffmanNodePtr nodeA(pq.top());
    pq.pop();
    HuffmanNodePtr nodeB(pq.top());
    pq.pop();
    pq.push(new HuffmanNode(std::move(nodeA), std::move(nodeB)));
  }

  root_ = HuffmanNodePtr(pq.top());
  pq.pop();

  // 生成编码
  tables_.fill("");
  generateCodes(root_.get(), "");
}

void HuffmanTree::generateCodes(HuffmanNode *node,
                                const std::string &code) noexcept {
  if (!node)
    return;

  if (node->isLeaf()) {
    tables_[node->symbol] = code;
    return;
  }

  if (node->left) {
    generateCodes(node->left.get(), code + "0");
  }
  if (node->right) {
    generateCodes(node->right.get(), code + "1");
  }
}

std::string HuffmanTree::encode(const std::vector<uint8_t> &bits) noexcept {
  std::string result;
  for (uint8_t bit : bits) {
    result.append(tables_[bit]);
  }
  return result;
}

std::string HuffmanTree::encode(const char *bits, size_t size) noexcept {
  std::string result;
  for (int i = 0; i < size; i++) {
    result.append(tables_[bits[i]]);
  }
  return result;
}

std::vector<uint8_t> HuffmanTree::decode(std::string_view data) noexcept {
  std::vector<uint8_t> result;
  auto *current = root_.get();

  for (uint8_t bit : data) {
    if (bit == 0) {
      current = current->left.get();
    } else {
      current = current->right.get();
    }

    if (current->isLeaf()) {
      result.emplace_back(current->symbol);
      current = root_.get();
    }
  }

  return result;
}

void HuffmanTree::printTree() { printNode(root_.get(), "", false); }

void HuffmanTree::printNode(const HuffmanTree::HuffmanNode *node,
                            const std::string &prefix, bool is_left) noexcept {
  if (!node) {
    return;
  }

  std::print("{} {}", prefix, is_left ? "├── " : "└── ");

  if (node->isLeaf()) {
    std::println("({}, {})", static_cast<char>(node->symbol), node->freq);

  } else {
    std::println("(*, {})", node->freq);
  }

  const std::string child_prefix = prefix + (is_left ? "│   " : "    ");

  if (node->left)
    printNode(node->left.get(), child_prefix, true);
  if (node->right)
    printNode(node->right.get(), child_prefix, false);
}

void HuffmanTree::printTable() {
  for (int i = 0; i < BYTE_SIZE; i++) {
    if (!tables_[i].empty()) {
      std::println("{:c}, {}", i, tables_[i]);
    }
  }
}

} // namespace leza::compression::huffman