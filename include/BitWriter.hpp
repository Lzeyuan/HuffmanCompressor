#include <bitset>
#include <fstream>
#include <vector>

#include "HuffmanTree.hpp"

namespace leza::compression::huffman {

class FastBitWriter {
private:
  std::vector<uint8_t> buffer; // 输出缓冲区 (减少 IO 次数)
  std::ofstream &outFile;

  //   uint64_t accumulator = 0; // 64位蓄水池，直接在寄存器操作
  std::bitset<HuffmanCode::BYTE_SIZE> accumulator =
      0;             // 64位蓄水池，直接在寄存器操作
  int bitsInAcc = 0; // 当前蓄了多少位

  static constexpr size_t FLUSH_THRESHOLD = 1024 * 1024; // 1MB 刷盘

public:
  explicit FastBitWriter(std::ofstream &os) : outFile(os) {
    buffer.reserve(FLUSH_THRESHOLD);
  }

  // 核心优化：直接接受整数位，而不是字符串
  // [[gnu::always_inline]] // 如果是 GCC/Clang，可以强制内联
  void writeCode(HuffmanCode code) {
    // 1. 把新的位加入蓄水池
    // 注意：这里假设是从低位向高位填，或者高位向低位，需保持一致
    // 这里演示：新数据放在高位 (Big-Endian stream 风格)
    // 但通常 ZIP/Deflate 是从 LSB (低位) 开始填，这里用 LSB 举例更通用：

    accumulator |= (code.val << bitsInAcc);
    bitsInAcc += code.len;

    // 2. 蓄水池满了（超过 8 位），就把“整字节”倒出来
    while (bitsInAcc >= 8) {
      buffer.push_back(static_cast<uint8_t>(accumulator & 0xFF));
      accumulator >>= 8; // 扔掉已写入的 8 位
      bitsInAcc -= 8;

      // 3. 缓冲区满了，刷盘
      if (buffer.size() >= FLUSH_THRESHOLD) {
        flushBuffer();
      }
    }
  }

  void flush() {
    // 写入残余的位
    if (bitsInAcc > 0) {
      buffer.push_back(static_cast<uint8_t>(accumulator & 0xFF));
      bitsInAcc = 0;
      accumulator = 0;
    }
    flushBuffer();
  }

private:
  void flushBuffer() {
    if (!buffer.empty()) {
      outFile.write(reinterpret_cast<const char *>(buffer.data()),
                    buffer.size());
      buffer.clear();
    }
  }
};
} // namespace leza::compression::huffman