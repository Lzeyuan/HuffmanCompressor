#include <cstdint>
#include <memory>

namespace leza::compression::huffman {

struct HuffmanNode {
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;

  uint8_t ch;
  uint64_t freq;
  HuffmanNodePtr left;
  HuffmanNodePtr right;

  HuffmanNode(uint8_t c, uint64_t f)
      : ch(c), freq(f), left(nullptr), right(nullptr) {}

  HuffmanNode(HuffmanNodePtr l, HuffmanNodePtr r)
      : ch(0), freq(l->freq + r->freq), left(std::move(l)),
        right(std::move(r)) {}
};

} // namespace leza::compression::huffman