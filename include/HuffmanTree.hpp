#include <bitset>

#include "HuffmanNode.hpp"

namespace leza::compression::huffman {

struct HuffmanCode {
  static constexpr int BYTE_SIZE = 256;
  std::bitset<BYTE_SIZE> val = 0;
  uint8_t len = 0;
};

class HuffmanTree {
public:
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;
  using CodeTable = std::array<HuffmanCode, HuffmanCode::BYTE_SIZE>;

  HuffmanNodePtr buildTree(const uint64_t freq[256]);
};

} // namespace leza::compression::huffman