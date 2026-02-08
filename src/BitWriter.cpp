#include "BitWriter.hpp"
#include <cassert>

namespace leza::compression::huffman {
void BitWriter::writeBit(bool bit) {
  buffer_ <<= 1;
  if (bit) {
    buffer_ |= 1;
  }
  ++bitCount_;

  if (bitCount_ == 8) {
    out.put(static_cast<char>(buffer_));
    buffer_ = 0;
    bitCount_ = 0;
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

int BitWriter::flush() {
  if (bitCount_ > 0) {
    int fillCount = (8 - bitCount_);
    buffer_ <<= fillCount;
    out.put(static_cast<char>(buffer_));
    buffer_ = 0;
    bitCount_ = 0;
    return fillCount;
  }
  return 0;
}

} // namespace leza::compression::huffman