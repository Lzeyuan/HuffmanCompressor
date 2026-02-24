#include <bit>
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

class IByteSinkTest : public ::testing::Test {
protected:
  void SetUp() override {
    ss_.str("");
    ss_.clear();
  }

  // 辅助函数：获取当前写入的字符串内容
  std::string getResult() { return ss_.str(); }

  std::stringstream ss_;
  leza::compression::huffman::util::OstreamSink sink_{ss_};
};

TEST_F(IByteSinkTest, WriteUInt16LittleEndian) {
  using namespace leza::compression::huffman::util;

  constexpr uint16_t expect = 10086;
  sink_.writeInLittleEndian(expect);

  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
}

TEST_F(IByteSinkTest, WriteUInt32LittleEndian) {
  using namespace leza::compression::huffman::util;

  constexpr uint32_t expect = 10086;
  sink_.writeInLittleEndian(expect);

  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[2]), expect >> 2 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[3]), expect >> 3 * 8 & 0xff);
}

TEST_F(IByteSinkTest, WriteUInt64LittleEndian) {
  using namespace leza::compression::huffman::util;

  constexpr uint64_t expect = 10086;
  sink_.writeInLittleEndian(expect);

  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[2]), expect >> 2 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[3]), expect >> 3 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[4]), expect >> 4 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[5]), expect >> 5 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[6]), expect >> 6 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[7]), expect >> 7 * 8 & 0xff);
}

TEST_F(IByteSinkTest, WriteInt16LittleEndian) {
  using namespace leza::compression::huffman::util;

  constexpr int16_t expect = 0X3F3F;
  sink_.writeInLittleEndian(expect);

  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
}

TEST_F(IByteSinkTest, WriteInt32LittleEndian) {
  using namespace leza::compression::huffman::util;

  constexpr int32_t expect = 0X3F3F3F3F;
  sink_.writeInLittleEndian(expect);

  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[2]), expect >> 2 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[3]), expect >> 3 * 8 & 0xff);
}

TEST_F(IByteSinkTest, WriteInt64LittleEndian) {
  using namespace leza::compression::huffman::util;

  constexpr int64_t expect = 0X3F3F3F3F;
  sink_.writeInLittleEndian(expect);

  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[2]), expect >> 2 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[3]), expect >> 3 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[4]), expect >> 4 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[5]), expect >> 5 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[6]), expect >> 6 * 8 & 0xff);
  EXPECT_EQ(static_cast<uint8_t>(ss_.str()[7]), expect >> 7 * 8 & 0xff);
}

TEST_F(IByteSinkTest, WriteFloat) {
  using namespace leza::compression::huffman::util;

  constexpr float input = 0.26f;
  sink_.writeInLittleEndian(input);

  using UintT = typename IByteSink::UintSelector<sizeof(input)>::type;
  UintT expect = std::bit_cast<UintT>(input);
  // 不是想省略代码，float之类的类型大小是未知的，看平台实现。
  // 一般float是四字节
  // ASSERT_EQ(static_cast<uint8_t>(ss_.str()[0]), expect & 0xff);
  // ASSERT_EQ(static_cast<uint8_t>(ss_.str()[1]), expect >> 8 & 0xff);
  // ASSERT_EQ(static_cast<uint8_t>(ss_.str()[2]), expect >> 2 * 8 & 0xff);
  // ASSERT_EQ(static_cast<uint8_t>(ss_.str()[3]), expect >> 3 * 8 & 0xff);
  for (int i = 0; i < sizeof(input); i++) {
    EXPECT_EQ(static_cast<uint8_t>(ss_.str()[i]), (expect >> i * 8) & 0xff);
  }

  // 同理
  // UintT restore = 0;
  // restore |= static_cast<uint8_t>(ss_.str()[3]);
  // restore <<= 8;
  // restore |= static_cast<uint8_t>(ss_.str()[2]);
  // restore <<= 8;
  // restore |= static_cast<uint8_t>(ss_.str()[1]);
  // restore <<= 8;
  // restore |= static_cast<uint8_t>(ss_.str()[0]);
  UintT restore = 0;
  for (int i = sizeof(input) - 1; i >= 0; --i) {
    restore <<= 8;
    restore |= static_cast<uint8_t>(ss_.str()[i]);
  }
  ASSERT_EQ(std::bit_cast<float>(restore), input);
}

TEST_F(IByteSinkTest, WriteDouble) {
  using namespace leza::compression::huffman::util;

  constexpr double input = 0.10086;
  sink_.writeInLittleEndian(input);

  using UintT = typename IByteSink::UintSelector<sizeof(input)>::type;
  UintT expect = std::bit_cast<UintT>(input);

  for (int i = 0; i < sizeof(input); i++) {
    EXPECT_EQ(static_cast<uint8_t>(ss_.str()[i]), (expect >> i * 8) & 0xff);
  }

  UintT restore = 0;
  for (int i = sizeof(input) - 1; i >= 0; --i) {
    restore <<= 8;
    restore |= static_cast<uint8_t>(ss_.str()[i]);
  }
  ASSERT_EQ(std::bit_cast<double>(restore), input);
}