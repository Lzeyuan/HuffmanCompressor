#include <cstdint>
#include <sstream>

#include <gtest/gtest.h>
#include <sys/types.h>

#include "BinaryIO.hpp"

class BitWriterTest : public ::testing::Test {
protected:
  void SetUp() override {
    ss_.str("");
    ss_.clear();
  }

  // 辅助函数：获取当前写入的字符串内容
  std::string getResult() { return ss_.str(); }

  std::stringstream ss_;
  leza::compression::huffman::util::OstreamSink sink_{ss_};
  leza::compression::huffman::util::BitWriter bitwriter_{sink_};
};

TEST_F(BitWriterTest, WriteEmpty) {
  int padding = bitwriter_.flush();
  EXPECT_EQ(padding, 0);
  EXPECT_TRUE(getResult().empty());
}

TEST_F(BitWriterTest, Write1Bit) {
  bitwriter_.writeBit(1);

  int padding = bitwriter_.flush();
  ASSERT_EQ(padding, 7);

  ASSERT_EQ(0b1000'0000, static_cast<uint8_t>(getResult()[0]));
}

TEST_F(BitWriterTest, Write3Bit) {
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(1);

  int padding = bitwriter_.flush();
  ASSERT_EQ(padding, 5);

  ASSERT_EQ(0b1010'0000, static_cast<uint8_t>(getResult()[0]));
}

TEST_F(BitWriterTest, WriteSingleFullByte) {
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(1);

  int padding = bitwriter_.flush();
  ASSERT_EQ(padding, 0);

  ASSERT_EQ(0b1011'1011, static_cast<uint8_t>(getResult()[0]));
}

TEST_F(BitWriterTest, WriteMultipleBytes) {
  // 0b1111'1111'00(00)
  for (int i = 0; i < 8; ++i) {
    bitwriter_.writeBit(true);
  }
  bitwriter_.writeBit(false);
  bitwriter_.writeBit(false);

  ASSERT_EQ(getResult().size(), 1);
  EXPECT_EQ(static_cast<unsigned char>(getResult()[0]), 0xFF);

  int padding = bitwriter_.flush();
  EXPECT_EQ(padding, 6);
  EXPECT_EQ(getResult().size(), 2);

  EXPECT_EQ(static_cast<unsigned char>(getResult()[1]), 0x00);
}

TEST_F(BitWriterTest, VerifyBitOrderLogic) {
  // 0b1000'0000
  bitwriter_.writeBit(1);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(0);
  bitwriter_.writeBit(0);

  ASSERT_EQ(getResult().size(), 1);
  EXPECT_EQ(static_cast<unsigned char>(getResult()[0]), 0b1000'0000);
}

TEST(IByteSinkTest, WriteFloat) {
  using namespace leza::compression::huffman::util;
  std::stringstream ss_;
  leza::compression::huffman::util::OstreamSink sink_{ss_};

  float input = 0.26f;
  sink_.writeInLittleEndian(0.26f);

  using UintT = typename IByteSink::UintSelector<sizeof(input)>::type;
  UintT expect = std::bit_cast<UintT>(input);
  ASSERT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  ASSERT_EQ(static_cast<uint8_t>(ss_.str()[1]), (expect) >> 8 & 0xff);
  ASSERT_EQ(static_cast<uint8_t>(ss_.str()[2]), (expect) >> 2 * 8 & 0xff);
  ASSERT_EQ(static_cast<uint8_t>(ss_.str()[3]), (expect) >> 3 * 8 & 0xff);
}