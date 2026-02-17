#include "Noncopyable.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

namespace leza::compression::huffman::classic {

struct HuffmanNode {
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

  uint8_t symbol;
  HuffmanNodePtr left;
  HuffmanNodePtr right;

  HuffmanNode(uint8_t c) noexcept : symbol(c), left(nullptr), right(nullptr) {}

  HuffmanNode(HuffmanNodePtr &&l, HuffmanNodePtr &&r) noexcept
      : symbol(0), left(std::move(l)), right(std::move(r)) {}

  bool isLeaf() const noexcept { return !left && !right; }
};

class ClassicHuffmanTreeEncoder {
public:
  struct EncodeResult {
    uint32_t compressSize;
    uint32_t serializeTreeSize;
    uint8_t compressPadding;
    EncodeResult() = delete;
    EncodeResult(uint32_t compressSize, uint32_t serializeTreeSize,
                 uint8_t compressPadding)
        : compressSize(compressSize), serializeTreeSize(serializeTreeSize),
          compressPadding(compressPadding) {}
  };

  static constexpr size_t BYTE_SIZE = 256;
  static constexpr size_t BUFFER_SIZE = 1024;
  using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;
  using CodeTable = std::array<std::string, BYTE_SIZE>;
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

  ClassicHuffmanTreeEncoder() = delete;

  static EncodeResult encode(std::istream &inputStream,
                             std::ostream &outputStream);

private:
  struct HuffmanCollector {
    std::vector<bool> structBits;
    std::vector<uint8_t> leaves;
    CodeTable codeTable;

    void onEnter(const HuffmanNode *node, const std::string &code);
  };

  static FrequencyArray getFrequencyArray(std::istream &inputStream);
  [[nodiscard]]
  static int serialize(std::ostream &os, const std::vector<bool> structBits,
                       const std::vector<uint8_t> leaves);
  [[nodiscard]]
  static HuffmanNodePtr buildTree(const FrequencyArray &frequencyArray);
  [[nodiscard]]
  static HuffmanCollector collectTreeByPreorder(const HuffmanNode *root);
};

class FileCompressor : private utility::NonCopyable {
public:
  static constexpr size_t BYTE_SIZE = 256;
  using Byte = uint8_t;
  using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;

  const uint32_t MAGIC_NUMBER = 0x113;
  const uint8_t VERSION_NUMBER = 0x1;

  // 返回补0数
  [[nodiscard]]
  int compress(const std::string &inputPath, const std::string &outputPath);
  [[nodiscard]]
  int compress(std::istream &inputStream, std::ostream &outputStream);

private:
  struct Header {
    uint64_t originFileSize;
    uint32_t huffmanTreeSize;
    uint32_t compressSize;
    uint32_t magicNumber;
    uint8_t version;
    uint8_t isDirectory;
    uint8_t compressPadding;
    uint8_t extend;
  };
  static constexpr size_t BUFFER_SIZE = 1024 * 1024;

  std::string inputPath_;
  std::string outputPath_;
  char buffer_[BUFFER_SIZE];

  static void wirteHeader(std::ostream &outputStream, Header header);

  template <typename T>
  static void writeInBigEnd(std::ostream &os, T value) {
    for (int i = sizeof(T) - 1; i >= 0; --i) {
      os.put(static_cast<char>((value >> (i * 8)) & 0xFF));
    }
  }
};
} // namespace leza::compression::huffman::classic