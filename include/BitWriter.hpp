#ifndef BITWRITER_HPP_
#define BITWRITER_HPP_

#include <cstdint>
#include <ostream>

namespace leza::compression::huffman {
class BitWriter {
public:
  explicit BitWriter(std::ostream &os) : out(os) {}

  void writeBit(bool bit);
  void writeBits(uint64_t bits, uint8_t count);
  void writeBits(std::string_view bits);
  void flush();

private:
  std::ostream &out;

  uint8_t buffer = 0;   // bit 缓冲
  uint8_t bitCount = 0; // 当前 buffer 中已有多少 bit（0~7）
};

} // namespace leza::compression::huffman
#endif // BITWRITER_HPP_