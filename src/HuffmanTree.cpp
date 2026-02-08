#include "HuffmanTree.hpp"

#include <memory>
#include <print>
#include <queue>
#include <span>
#include <string>

namespace leza::compression::huffman {
HuffmanTree::Result
HuffmanTree::build(std::span<const uint8_t> bytes) noexcept {
  FrequencyArray frequencies;
  for (uint8_t byte : bytes) {
    frequencies[byte]++;
  }
  return HuffmanTree::build(frequencies);
}

HuffmanTree::Result HuffmanTree::build(const FrequencyArray &freq) noexcept {
  HuffmanNodePtr root = buildTree(freq);
  CodeTable tables = generateCodeTable(root.get());
  return {tables, std::move(root)};
}

HuffmanTree::HuffmanNodePtr
HuffmanTree::buildTree(const FrequencyArray &freq) noexcept {
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

  auto root = HuffmanNodePtr(pq.top());
  return root;
}

HuffmanTree::CodeTable
HuffmanTree::generateCodeTable(HuffmanNode *root) noexcept {
  CodeTable ret;
  auto generate = [&ret](this auto &&generate, HuffmanNode *node,
                         const std::string &code) -> void {
    if (!node)
      return;

    if (node->isLeaf()) {
      ret[node->symbol] = code;
      return;
    }

    if (node->left) {
      generate(node->left.get(), code + "0");
    }
    if (node->right) {
      generate(node->right.get(), code + "1");
    }
  };
  generate(root, "");
  return ret;
}

// std::string HuffmanTree::encode(const std::vector<uint8_t> &bytes) noexcept {
//   std::string result;
//   for (uint8_t byte : bytes) {
//     result.append(tables_[byte]);
//   }
//   return result;
// }

// std::string HuffmanTree::encode(const char *bytes, size_t size) noexcept {
//   std::string result;
//   for (int i = 0; i < size; i++) {
//     result.append(tables_[static_cast<uint8_t>(bytes[i])]);
//   }
//   return result;
// }

// std::vector<uint8_t> HuffmanTree::decode(std::string_view data) noexcept {
//   std::vector<uint8_t> result;
//   auto *current = root_.get();

//   for (uint8_t bit : data) {
//     if (bit == 0) {
//       current = current->left.get();
//     } else {
//       current = current->right.get();
//     }

//     if (current->isLeaf()) {
//       result.emplace_back(current->symbol);
//       current = root_.get();
//     }
//   }

//   return result;
// }

void HuffmanTree::printTree(const HuffmanNode *const node) {
  printNode(node, "", false);
}

void HuffmanTree::printNode(const HuffmanNode *const node,
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

} // namespace leza::compression::huffman