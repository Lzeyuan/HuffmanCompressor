#include "Noncopyable.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <sys/stat.h>
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
  static constexpr size_t BYTE_SIZE = 256;
  static constexpr size_t BUFFER_SIZE = 1024;
  using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;
  using CodeTable = std::array<std::string, BYTE_SIZE>;
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

  ClassicHuffmanTreeEncoder() = delete;

  static int encode(std::istream &inputStream, std::ostream &outputStream,
                    const FrequencyArray &frequencyArray);

private:
  struct HuffmanCollector {
    std::vector<bool> structBits;
    std::vector<uint8_t> leaves;
    CodeTable codeTable;

    // 进入节点时的处理逻辑
    void onEnter(const HuffmanNode *node, const std::string &code);
  };
  [[nodiscard]]
  static int serialize(std::ostream &os, const HuffmanNode *root);
  [[nodiscard]]
  static HuffmanNodePtr buildTree(const FrequencyArray &frequencyArray);
  [[nodiscard]]
  static HuffmanCollector collectTreeByPreorder(const HuffmanNode *root);
  static FrequencyArray getFrequencyArray(std::istream &inputStream,
                                          uint64_t &fileSize);
  void buildSerializationArrays(const HuffmanNode *node,
                                std::vector<bool> &structBits,
                                std::vector<uint8_t> &leaves);
  void bits2BytesWithMSBF(const std::vector<bool> &bits,
                          std::vector<uint8_t> &out);
  void serializeTree2Stream(const HuffmanNode *root, std::ostream &os);
  void write4Byte(std::ostream &os, uint32_t v);
};

struct Header {
  uint64_t originFileSize;
  uint32_t huffmanTreeSize;
  uint32_t compressSize;
  uint32_t magicNumber;
  uint8_t version;
  uint8_t isDirectory;
  uint16_t extend;
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
  static constexpr size_t BUFFER_SIZE = 1024 * 1024;

  std::string inputPath_;
  std::string outputPath_;
  char buffer_[BUFFER_SIZE];

  [[nodiscard]]
  static FrequencyArray getFrequencyArray(std::istream &inputStream,
                                          uint64_t &fileSize);
  void buildSerializationArrays(const HuffmanNode *node,
                                std::vector<bool> &structBits,
                                std::vector<Byte> &leaves);
  void bits2BytesWithMSBF(const std::vector<bool> &bits,
                          std::vector<Byte> &out);
  void serializeTree2Stream(const HuffmanNode *root, std::ostream &os);
  void write4Byte(std::ostream &os, uint32_t v);
};
} // namespace leza::compression::huffman::classic