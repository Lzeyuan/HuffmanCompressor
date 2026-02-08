#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "HuffmanCompressor.hpp"

class HuffmanCompressorTest : public ::testing::Test {
protected:
  std::filesystem::path dir;

  void SetUp() override {
    auto base = std::filesystem::temp_directory_path();
    dir = base /
          ("gtest-" +
           std::to_string(
               std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(dir);
  }

//   void TearDown() override { std::filesystem::remove_all(dir); }
};

// TEST_F(HuffmanCompressorTest, WriteFile) {
//   auto file = dir / "data.bin";

//   std::ofstream ofs(file, std::ios::binary);
//   ofs << "abc";

//   EXPECT_TRUE(std::filesystem::exists(file));
// }

TEST_F(HuffmanCompressorTest, compress) {
  using namespace leza::compression::huffman;
  std::string inputPath =
      "/home/leza/projects/huffman-compressor/test/test_files/random_data.bin";
  std::string outputPath = dir / "outData.bin";

  FileCompressor fileCompressor(inputPath, outputPath);
  fileCompressor.compress();
}
