#ifndef HUFFMANETREE_HPP_
#define HUFFMANETREE_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>

namespace leza::compression::huffman {

class HuffmanTree {
public:
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

  static constexpr size_t BYTE_SIZE = 256;
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;
  using CodeTable = std::array<std::string, BYTE_SIZE>;
  using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;

  HuffmanTree() = delete;
  ~HuffmanTree() = delete;
  HuffmanTree(const HuffmanTree &) = delete;
  HuffmanTree &operator=(const HuffmanTree &) = delete;
  HuffmanTree(HuffmanTree &&) noexcept = delete;
  HuffmanTree &operator=(HuffmanTree &&) noexcept = delete;

  [[nodiscard]] static HuffmanNodePtr buildTree(const FrequencyArray &freq) noexcept;
  [[nodiscard]] static CodeTable buildCodeTable(HuffmanNode *root) noexcept;
  [[nodiscard]] static CodeTable buildCodeTable(const FrequencyArray &freq) noexcept;

  static void printTree(const HuffmanNode *const node);

private:
  static CodeTable generateCodeTable(HuffmanNode *root) noexcept;
  static void printNode(const HuffmanNode *const node,
                        const std::string &prefix, bool is_left) noexcept;
};
} // namespace leza::compression::huffman
#endif // HUFFMANETREE_HPP_