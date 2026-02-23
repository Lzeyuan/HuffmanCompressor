#include <gtest/gtest.h>

#include <cstddef>
#include <print>
#include <span>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(HelloTest, SpanTest) {
  char chars[1024];
  size_t size = 10;
  for (int i = 0; i < size; i++) {
    chars[i] = '0' + i;
  }
  auto bytes = std::as_bytes(std::span<char>{chars, size});
  for (auto b : bytes) {
    std::print("{} ", static_cast<char>(b));
  }
  std::println();
}