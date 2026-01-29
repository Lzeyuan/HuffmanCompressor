#include "HuffmanTree.hpp"
#include <memory>
#include <queue>

namespace leza::compression::huffman {
std::unique_ptr<HuffmanNode> HuffmanTree::buildTree(const uint64_t freq[256]) {
  using std::priority_queue;
  using std::unique_ptr;
  using std::vector;
  using HuffmanNodePtr = std::unique_ptr<HuffmanNode>;
  using HuffmanNodeRaw = HuffmanNode *;

  auto huffmanNodeCmp = [](const HuffmanNode *a, const HuffmanNode *b) -> bool {
    return a->freq > b->freq;
  };

  priority_queue<HuffmanNode *, vector<HuffmanNode *>, decltype(huffmanNodeCmp)>
      pq;

  for (int i = 0; i < 256; ++i) {
    if (freq[i] > 0)
      pq.push(new HuffmanNode((unsigned char)i, freq[i]));
  }

  // 单字符文件兜底
  if (pq.size() == 1) {
    uint8_t dummyChar = 0;
    while (freq[dummyChar] > 0) {
      dummyChar++; // 找一个文件里没有的符号
    }
    pq.push(new HuffmanNode(dummyChar, 0)); // dummy 节点
  }

  while (pq.size() > 1) {
    HuffmanNodePtr nodeA(pq.top());
    pq.pop();
    HuffmanNodePtr nodeB(pq.top());
    pq.pop();
    pq.push(new HuffmanNode(std::move(nodeA), std::move(nodeB)));
  }

  HuffmanNodeRaw rootPtr = pq.top();
  pq.pop();
  return HuffmanNodePtr(rootPtr);
}
} // namespace leza::compression::huffman