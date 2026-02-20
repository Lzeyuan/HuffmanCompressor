#include "BitWriter.hpp"

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