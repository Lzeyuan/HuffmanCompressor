#pragma once
#ifndef HUFFMANCOMPRESSOR_HPP_
#define HUFFMANCOMPRESSOR_HPP_

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

#include "BinaryIO.hpp"

// https://www.rfc-editor.org/rfc/rfc1952

namespace leza::compression::huffman {
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

class CRC {
public:
  CRC() {
    uint32_t c;
    int n, k;
    for (n = 0; n < 256; n++) {
      c = (uint32_t)n;
      for (k = 0; k < 8; k++) {
        if (c & 1) {
          c = 0xedb88320L ^ (c >> 1);
        } else {
          c = c >> 1;
        }
      }
      crc_table[n] = c;
    }
  }

  [[nodiscard]]
  uint32_t crc(unsigned char *buf, int len) noexcept {
    return update_crc(0L, buf, len);
  }

private:
  uint32_t crc_table[256];

  uint32_t update_crc(uint32_t crc, unsigned char *buf, int len) noexcept {
    uint32_t c = crc ^ 0xffffffffL;
    int n;

    for (n = 0; n < len; n++) {
      c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c ^ 0xffffffffL;
  }
};
} // namespace core

struct GZipHeader {
  static constexpr uint8_t ID1{0X1F};
  static constexpr uint8_t ID2{0X8B};
  uint8_t CompressionMethod{8};
  uint8_t FLaGs;
  uint32_t ModificationTIME;
  uint8_t ExtraFLags;
  uint8_t OS;
};

struct EncodeResult {
  uint32_t compressSize;
  std::vector<uint8_t> data;
};

struct DecodeResult {
  std::vector<uint8_t> data;
};

inline void encode(util::IByteReader &reader, util::IByteWriter &writer) {
  GZipHeader header;
  header.FLaGs = 0b100 | 0b1000;

  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
  header.ModificationTIME = static_cast<uint32_t>(seconds.count());

  header.ExtraFLags = 4;
  header.OS = 3;

  writer.writeByte(header.ID1);
  writer.writeByte(header.ID2);
  writer.writeByte(header.CompressionMethod);
  writer.writeByte(header.FLaGs);
  writer.writeByte(header.ModificationTIME);

  
}

} // namespace leza::compression::huffman

#endif // HUFFMANCOMPRESSOR_HPP_