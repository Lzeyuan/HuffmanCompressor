#include "HuffmanTree.hpp"

#include <cstdint>
#include <memory>
#include <print>
#include <queue>
#include <string>
#include <utility>

namespace leza::compression::huffman {
HuffmanTree::HuffmanNodePtr
HuffmanTree::buildTree(const FrequencyArray &freq) noexcept {
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;
  using FrequencyNode = std::pair<uint64_t, HuffmanNodePtr>;

  auto huffmanNodeCmp = [](const FrequencyNode &a,
                           const FrequencyNode &b) -> bool {
    return a.first > b.first;
  };

  std::priority_queue<FrequencyNode, std::vector<FrequencyNode>,
                      decltype(huffmanNodeCmp)>
      pq;

  for (int i = 0; i < 256; ++i) {
    if (freq[i] > 0) {
      pq.push({freq[i], std::make_unique<HuffmanNode>(i)});
    }
  }

  if (pq.size() == 0) {
    return nullptr;
  }

  // 单字符文件兜底
  if (pq.size() == 1) {
    uint8_t dummyChar = freq[0] > 0 ? 1 : 0;
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

HuffmanTree::CodeTable HuffmanTree::buildCodeTable(HuffmanNode *root) noexcept {
  return generateCodeTable(root);
}

HuffmanTree::CodeTable
HuffmanTree::buildCodeTable(const FrequencyArray &freq) noexcept {
  return generateCodeTable(buildTree(freq).get());
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

  std::print("{}{}", prefix, is_left ? "├── " : "└── ");

  if (node->isLeaf()) {
    std::println("({})", static_cast<uint8_t>(node->symbol));
  } else {
    std::println("[{}]", is_left ? "0" : "1");
  }

  const std::string child_prefix = prefix + (is_left ? "│   " : "    ");

  if (node->left)
    printNode(node->left.get(), child_prefix, true);
  if (node->right)
    printNode(node->right.get(), child_prefix, false);
}

} // namespace leza::compression::huffman