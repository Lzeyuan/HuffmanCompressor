#ifndef HUFFMAN_COMPRESSOER_HPP_
#define HUFFMAN_COMPRESSOER_HPP_

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace leza::compression::huffman::classic {
namespace core {

struct HuffmanNode;

constexpr size_t BYTE_SIZE = 256;
using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;
using CodeTable = std::array<std::string, BYTE_SIZE>;
using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

struct HuffmanNode {
  uint8_t symbol;
  HuffmanNodePtr left;
  HuffmanNodePtr right;

  HuffmanNode(uint8_t c) noexcept : symbol(c), left(nullptr), right(nullptr) {}

  HuffmanNode(HuffmanNodePtr &&l, HuffmanNodePtr &&r) noexcept
      : symbol(0), left(std::move(l)), right(std::move(r)) {}

  bool isLeaf() const noexcept { return !left && !right; }
};

struct HuffmanCollector {
  std::vector<uint8_t> structBits;
  std::vector<uint8_t> leaves;
  CodeTable codeTable;

  void onNode(const HuffmanNode *node,
              const std::vector<uint8_t> &code) noexcept {
    if (node->isLeaf()) {
      structBits.push_back(true);
      leaves.push_back(node->symbol);
      codeTable[node->symbol].append_range(code);
    } else {
      structBits.push_back(false);
    }
  }
};

// ===== encode =====
struct SerializeResult {
  std::vector<uint8_t> data;
  uint32_t size;
};

[[nodiscard]]
FrequencyArray getFrequencyArray(std::span<uint8_t> data);

[[nodiscard]]
HuffmanNodePtr buildTree(const FrequencyArray &frequencyArray);

[[nodiscard]]
HuffmanCollector collectTreeByPreorder(const HuffmanNode *root);

[[nodiscard]]
SerializeResult serialize(const std::vector<uint8_t> structBits,
                          const std::vector<uint8_t> leaves);
// ===== encode =====

// ===== decode =====
struct DeSerializeResult {
  uint32_t readCount;
  HuffmanNodePtr root;
};

[[nodiscard]]
DeSerializeResult deserialize(std::span<uint8_t> data);
// ===== decode =====
} // namespace core

struct EncodeResult {
  uint32_t compressSize;
  std::vector<uint8_t> data;
};

struct DecodeResult {
  std::vector<uint8_t> data;
};

constexpr size_t BUFFER_SIZE = 1024;

[[nodiscard]]
EncodeResult encode(std::span<uint8_t> inputData);

[[nodiscard]]
DecodeResult decode(std::span<uint8_t> inputData);

} // namespace leza::compression::huffman::classic

#endif // HUFFMAN_COMPRESSOER_HPP_