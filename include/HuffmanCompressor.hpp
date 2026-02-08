#include <cstdint>
#include <string>
#include <vector>

#include "HuffmanTree.hpp"

namespace leza::compression::huffman::simple {

class Encoder {
public:
  static constexpr size_t BYTE_SIZE = 256;
  using CodeTable = std::array<std::string, BYTE_SIZE>;

  Encoder(const CodeTable &codeTable_) : codeTable_(codeTable_) {}
  ~Encoder() = default;
  Encoder(const Encoder &) = delete;
  Encoder &operator=(const Encoder &) = delete;
  Encoder(Encoder &&) noexcept = default;
  Encoder &operator=(Encoder &&) noexcept = default;

  std::string encode(const std::byte byte);

private:
  CodeTable codeTable_;
};

class Decoder {
public:
  using HuffmanNodePtr = HuffmanTree::HuffmanNodePtr;

  Decoder(HuffmanNodePtr &&huffmanNodePtr);
  ~Decoder() = default;
  Decoder(const Decoder &) = delete;
  Decoder &operator=(const Decoder &) = delete;
  Decoder(Decoder &&) noexcept = default;
  Decoder &operator=(Decoder &&) noexcept = default;

  bool decode(bool bit, std::byte &outByte);

private:
  HuffmanTree::HuffmanNodePtr root_;
  HuffmanTree::HuffmanNode *current_;
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

class FileCompressor {
public:
  using HuffmanNodePtr = HuffmanTree::HuffmanNodePtr;
  using CodeTable = HuffmanTree::CodeTable;
  FileCompressor() = default;
  ~FileCompressor() = default;

  // static std::unique_ptr<FileCompressor> create(const std::string &inputPath,
  //                                               const std::string
  //                                               &outputPath);

  int compress(const std::string &inputPath, const std::string &outputPath);
  void decompress(const std::string &out);

private:
  static constexpr size_t BUFFER_SIZE = 1024 * 1024;

  std::string inputPath_;
  std::string outputPath_;
  char buffer_[BUFFER_SIZE];

  // Encoder encoder_;
  // Decoder decoder_;

  FileCompressor(const FileCompressor &) = delete;
  FileCompressor &operator=(const FileCompressor &) = delete;

  // static HuffmanTree::FrequencyArray
  // getFrequencyArray(const std::string &inputPath);
  [[nodiscard]] static HuffmanTree::FrequencyArray
  getFrequencyArray(std::ifstream &inputStream);
};
} // namespace leza::compression::huffman::simple