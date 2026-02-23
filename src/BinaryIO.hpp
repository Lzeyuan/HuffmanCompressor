#ifndef WRITER_HPP_
#define WRITER_HPP_

#include <cstdint>
#include <ostream>
#include <span>
#include <type_traits>

// 目前写入有两种，写文件和写字符串，正好对应 iostream 的两个子类，
// 也就说其实把 iostream 作为接口参数就行了，但是可能后期会优化性能，
// 所以还是加一层 IByteSink。
namespace leza::compression::huffman::util {
class IByteSink {
public:
  virtual ~IByteSink() = default;

  virtual void writeByte(uint8_t byte) = 0;

  virtual void writeBytes(std::span<const uint8_t> data) {
    for (uint8_t byte : data) {
      writeByte(byte);
    }
  }

  // clang-format off
  template <size_t N> struct UintSelector;
  template <> struct UintSelector<1> { using type = uint8_t; };
  template <> struct UintSelector<2> { using type = uint16_t; };
  template <> struct UintSelector<4> { using type = uint32_t; };
  template <> struct UintSelector<8> { using type = uint64_t; };
  // clang-format on

  template <typename T>
    requires std::is_arithmetic_v<T> && (sizeof(T) > 1)
  void writeInLittleEndian(T value) {
    using UintT = typename UintSelector<sizeof(T)>::type;
    UintT u_val = std::bit_cast<UintT>(value);

    constexpr size_t numBytes = sizeof(T);
    for (size_t i = 0; i < numBytes; ++i) {
      uint8_t byte = static_cast<uint8_t>(u_val >> (i * 8));
      writeByte(byte);
    }
  }
};

class OstreamSink final : public IByteSink {
public:
  OstreamSink(std::ostream &os) : os_(os) {}

  void writeByte(uint8_t byte) override { os_.put(static_cast<char>(byte)); }

  void writeBytes(std::span<const uint8_t> data) override {
    os_.write(reinterpret_cast<const char *>(data.data()), data.size());
  }

private:
  std::ostream &os_;
};

class BitWriter final {
public:
  explicit BitWriter(IByteSink &byteSink) : out_(byteSink) {}

  void writeBit(bool bit) {
    buffer_ <<= 1;
    buffer_ |= bit;
    ++bitCount_;

    if (bitCount_ == 8) {
      out_.writeByte(static_cast<char>(buffer_));
      buffer_ = 0;
      bitCount_ = 0;
    }
  }
  int flush() {
    if (bitCount_ > 0) {
      int fillCount = (8 - bitCount_);
      buffer_ <<= fillCount;
      out_.writeByte(static_cast<char>(buffer_));
      buffer_ = 0;
      bitCount_ = 0;
      return fillCount;
    }
    return 0;
  }

private:
  IByteSink &out_;

  uint8_t buffer_ = 0;   // bit 缓冲
  uint8_t bitCount_ = 0; // 当前 buffer 中已有多少 bit（0~7）
};

} // namespace leza::compression::huffman::util
#endif // WRITER_HPP_