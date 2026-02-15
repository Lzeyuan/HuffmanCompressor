// #ifndef HUFFMANETREE_HPP_
// #define HUFFMANETREE_HPP_

// #include <array>
// #include <cstddef>
// #include <cstdint>
// #include <expected>
// #include <memory>
// #include <ostream>
// #include <string>
// #include <unordered_map>

// #include "Noncopyable.hpp"

// namespace leza::compression::huffman::classic {
// // struct HuffmanNode {
// //   using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

// //   uint8_t symbol;
// //   HuffmanNodePtr left;
// //   HuffmanNodePtr right;

// //   HuffmanNode(uint8_t c) noexcept : symbol(c), left(nullptr), right(nullptr) {}

// //   HuffmanNode(HuffmanNodePtr &&l, HuffmanNodePtr &&r) noexcept
// //       : symbol(0), left(std::move(l)), right(std::move(r)) {}

// //   bool isLeaf() const noexcept { return !left && !right; }
// // };

// // class ClassicHuffmanTreeEncoder {
// // public:
// //   static constexpr size_t BYTE_SIZE = 256;
// //   using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;
// //   using CodeTable = std::array<std::string, BYTE_SIZE>;
// //   using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

// //   ClassicHuffmanTreeEncoder() = delete;

// //   static int encode(std::ostream &os, const FrequencyArray &frequencyArray);

// // private:
// //   static void serialize(std::ostream &os);
// //   [[nodiscard]] static HuffmanNodePtr
// //   buildTree(const FrequencyArray &frequencyArray);
// //   [[nodiscard]] static CodeTable buildCodeTable(HuffmanNode *node);
// // };

// // class ClassicHuffmanTreeDecoder : private utility::MoveOnly {
// // public:
// //   static constexpr size_t BYTE_SIZE = 256;
// //   using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;
// //   using CodeTable = std::array<std::string, BYTE_SIZE>;
// //   using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

// //   static std::expected<ClassicHuffmanTreeDecoder, std::string>
// //   create(const FrequencyArray &frequencyArray);

// //   void encode(std::ostream &os);

// // private:
// //   ClassicHuffmanTreeDecoder() = default;
// //   void deserialize(std::ostream &os);

// //   std::unordered_map<std::string, uint8_t> decodeTable_;
// // };

// // class HuffmanTree : private utility::NonCopyable {
// // public:
// //   static constexpr size_t BYTE_SIZE = 256;
// //   using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;
// //   using CodeTable = std::array<std::string, BYTE_SIZE>;
// //   using FrequencyArray = std::array<uint64_t, BYTE_SIZE>;

// //   [[nodiscard]] static HuffmanNodePtr
// //   buildTree(const FrequencyArray &freq) noexcept;
// //   [[nodiscard]] static CodeTable buildCodeTable(HuffmanNode *root) noexcept;
// //   [[nodiscard]] static CodeTable
// //   buildCodeTable(const FrequencyArray &freq) noexcept;

// //   static void printTree(const HuffmanNode *const node);

// // private:
// //   static CodeTable generateCodeTable(HuffmanNode *root) noexcept;
// //   static void printNode(const HuffmanNode *const node,
// //                         const std::string &prefix, bool is_left) noexcept;
// // };
// } // namespace leza::compression::huffman::classic
// #endif // HUFFMANETREE_HPP_