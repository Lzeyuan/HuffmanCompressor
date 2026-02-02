#include "BitWriter.hpp"
#include <cassert>

namespace leza::compression::huffman {
void BitWriter::writeBit(bool bit) {
  buffer <<= 1;
  if (bit) {
    buffer |= 1;
  }
  ++bitCount;

  if (bitCount == 8) {
    out.put(static_cast<char>(buffer));
    buffer = 0;
    bitCount = 0;
  }
}

void BitWriter::writeBits(uint64_t bits, uint8_t count) {
  // 从高位往低位写
  for (int i = count - 1; i >= 0; --i) {
    writeBit((bits >> i) & 1);
  }
}

void BitWriter::writeBits(std::string_view bits) {
  for (int i = bits.size() - 1; i >= 0; --i) {
    assert(bits[i] == '0' || bits[i] == '1');
    writeBit(static_cast<bool>(bits[i] - '0'));
  }
}

void BitWriter::flush() {
  if (bitCount > 0) {
    buffer <<= (8 - bitCount); // 低位补 0
    out.put(static_cast<char>(buffer));
    buffer = 0;
    bitCount = 0;
  }
}

} // namespace leza::compression::huffman