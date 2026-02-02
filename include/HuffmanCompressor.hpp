#include <array>
#include <cstdint>
#include <fstream>
#include <vector>

#include "BitWriter.hpp"
#include "HuffmanTree.hpp"

namespace leza::compression::huffman {

constexpr int REVERSE_SIZE = 8;
#pragma pack(push, 1)
struct FileHeader {
  uint32_t magicnUmber;
  uint8_t version;
  uint64_t originSize;
  uint32_t treeSize;
  uint32_t compressionSize;
  uint8_t type;
  uint8_t reverse[REVERSE_SIZE];
};
#pragma pack(pop)

class HuffmanArchiver {
public:
  std::vector<uint8_t> compress(std::string_view inputPath,
                                std::string_view out);

private:
  std::vector<uint8_t> serializeHeader(const FileHeader &header);
};

class FileCompressor {

public:
  FileCompressor(std::string_view in, std::string_view out)
      : inputPath_(in), outputPath_(out) {}

  void compress();

private:
  static const size_t BUFFER_SIZE = 1024 * 1024;

  std::string inputPath_;
  std::string outputPath_;
  HuffmanTree huffmanTree_;
  HuffmanTree::FrequencyArray frequencies_;
};
} // namespace leza::compression::huffman