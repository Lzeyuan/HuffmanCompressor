#ifndef HUFFMANETREE_HPP_
#define HUFFMANETREE_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace leza::compression::huffman {

class HuffmanTree {
private:
  struct HuffmanNode;

public:
  static constexpr size_t BYTE_SIZE = 256;
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;
  using CodeTable = std::array<std::string, BYTE_SIZE>;
  using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;

  HuffmanTree() = default;
  ~HuffmanTree() = default;

  HuffmanTree(const HuffmanTree &) = delete;
  HuffmanTree &operator=(const HuffmanTree &) = delete;
  HuffmanTree(HuffmanTree &&) noexcept = default;
  HuffmanTree &operator=(HuffmanTree &&) noexcept = default;

  void buildTree(const FrequencyArray &freq) noexcept;
  void buildTree(const uint8_t *bytes, size_t size) noexcept;

  std::string encode(const std::vector<uint8_t> &bits) noexcept;
  std::string encode(const char *bits, size_t size) noexcept;

  std::vector<uint8_t> decode(std::string_view data) noexcept;

  void printTree();
  void printTable();

private:
  struct HuffmanNode {
    using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

    uint8_t symbol;
    uint64_t freq;
    HuffmanNodePtr left;
    HuffmanNodePtr right;

    HuffmanNode(uint8_t c, uint64_t f) noexcept
        : symbol(c), freq(f), left(nullptr), right(nullptr) {}

    HuffmanNode(HuffmanNodePtr &&l, HuffmanNodePtr &&r) noexcept
        : symbol(0), freq(l->freq + r->freq), left(std::move(l)),
          right(std::move(r)) {}

    bool isLeaf() const noexcept { return !left && !right; }
  };

  HuffmanNodePtr root_;
  CodeTable tables_;

  void generateCodes(HuffmanNode *node, const std::string &code) noexcept;
  void printNode(const HuffmanTree::HuffmanNode *node,
                 const std::string &prefix, bool is_left) noexcept;
};
} // namespace leza::compression::huffman
#endif // HUFFMANETREE_HPP_