#include "HuffmanCompressor.hpp"

#include <cstdint>
#include <memory>
#include <print>
#include <queue>
#include <utility>
#include <vector>

namespace leza::compression::huffman::classic {
namespace core {
[[nodiscard]]
FrequencyArray getFrequencyArray(std::span<uint8_t> data) {
  FrequencyArray ret{};
  for (auto d : data) {
    ret[d]++;
  }
  return ret;
}

[[nodiscard]]
HuffmanNodePtr buildTree(const FrequencyArray &frequencyArray) {
  using FrequencyNode = std::pair<uint64_t, HuffmanNodePtr>;

  auto huffmanNodeCmp = [](const FrequencyNode &a,
                           const FrequencyNode &b) -> bool {
    return a.first > b.first;
  };

  std::priority_queue<FrequencyNode, std::vector<FrequencyNode>,
                      decltype(huffmanNodeCmp)>
      pq;

  for (int i = 0; i < 256; ++i) {
    if (frequencyArray[i] > 0) {
      pq.push({frequencyArray[i], std::make_unique<HuffmanNode>(i)});
    }
  }

  if (pq.size() == 0) {
    return nullptr;
  }

  // 单字符文件兜底
  if (pq.size() == 1) {
    uint8_t dummyChar = frequencyArray[0] > 0 ? 1 : 0;
    pq.push({0, std::make_unique<HuffmanNode>(dummyChar)});
  }

  while (pq.size() > 1) {
    auto [freq1, left] = std::move(const_cast<FrequencyNode &>(pq.top()));
    pq.pop();

    auto [freq2, right] = std::move(const_cast<FrequencyNode &>(pq.top()));
    pq.pop();

    auto new_node =
        std::make_unique<HuffmanNode>(std::move(left), std::move(right));
    pq.push({freq1 + freq2, std::move(new_node)});
  }

  auto [_, root] = std::move(const_cast<FrequencyNode &>(pq.top()));
  return std::move(root);
}

[[nodiscard]]
HuffmanCollector collectTreeByPreorder(const HuffmanNode *root) {
  HuffmanCollector huffmanCollector;
  std::vector<uint8_t> code;
  auto generate = [&code](this auto &&generate, const HuffmanNode *node,
                          HuffmanCollector &huffmanCollector) -> void {
    if (!node) {
      return;
    }
    huffmanCollector.onNode(node, code);

    code.emplace_back(0);
    generate(node->left.get(), huffmanCollector);
    code.pop_back();

    code.emplace_back(1);
    generate(node->right.get(), huffmanCollector);
    code.pop_back();
  };
  generate(root, huffmanCollector);
  return huffmanCollector;
}

[[nodiscard]]

SerializeResult serialize(const std::vector<uint8_t> structBits,
                          const std::vector<uint8_t> leaves) {
  uint16_t structsLen = structBits.size();
  uint16_t leavesLen = leaves.size();

  SerializeResult result;

  auto writeUint16 = [&result](uint16_t data) {
    result.data.emplace_back(static_cast<char>(data >> 8));
    result.data.emplace_back(static_cast<char>(data & 0xFF));
  };

  // 叶子
  writeUint16(leavesLen);
  result.data.append_range(leaves);
  result.size = 2 + 2 + structsLen + leavesLen;

  // 树
  writeUint16(structsLen);
  for (auto b : structBits) {
    result.data.emplace_back(static_cast<char>(b));
  }

  return result;
}

[[nodiscard]]
DeSerializeResult deserialize(std::span<uint8_t> data) {
  uint32_t readCount = 0;
  uint16_t leavesLen = data[0] << 8 | data[1];
  readCount += 2;

  std::vector<uint8_t> leaves;
  for (int i = 0; i < leavesLen; i++) {
    leaves.emplace_back(data[readCount++]);
  }

  uint16_t structsLen = data[readCount] << 8 | data[readCount + 1];
  readCount += 2;

  int leavesIdx = 0;
  auto getHuffmanTree = [&](this auto &&getHuffmanTree) -> HuffmanNodePtr {
    if (data[readCount++] == 1) {
      return std::make_unique<HuffmanNode>(leaves[leavesIdx++]);
    } else {
      auto left = getHuffmanTree();
      auto right = getHuffmanTree();
      return std::make_unique<HuffmanNode>(std::move(left), std::move(right));
    }
  };

  DeSerializeResult ret;
  ret.root = getHuffmanTree();
  ret.readCount = readCount;
  return ret;
}

} // namespace core

[[nodiscard]]
EncodeResult encode(std::span<uint8_t> inputData) {
  using namespace leza::compression::huffman::classic::core;

  FrequencyArray frequencies = getFrequencyArray(inputData);
  auto root = buildTree(frequencies);
  auto huffmanCollector = collectTreeByPreorder(root.get());

  // 1.压缩头，树信息
  uint32_t compressByteCount = 0;
  auto SerializeResult =
      serialize(huffmanCollector.structBits, huffmanCollector.leaves);
  compressByteCount += SerializeResult.size;

  // 2.压缩文件
  std::vector<uint8_t> encodeBits;
  encodeBits.append_range(SerializeResult.data);
  for (auto byte : inputData) {
    encodeBits.append_range(huffmanCollector.codeTable[byte]);
  }

  return {compressByteCount, encodeBits};
}

[[nodiscard]]
DecodeResult decode(std::span<uint8_t> inputData) {
  auto deSerializeResult = core::deserialize(inputData);
  uint32_t readCount = deSerializeResult.readCount;

  std::vector<uint8_t> result;
  auto dataSpan = inputData.subspan(readCount);
  auto root = std::move(deSerializeResult.root);
  auto current = root.get();
  for (auto bit : dataSpan) {
    if (bit == 0) {
      current = current->left.get();
    } else {
      current = current->right.get();
    }

    if (current->isLeaf()) {
      result.emplace_back(current->symbol);
      current = root.get();
    }
  }
  return {result};
}

} // namespace leza::compression::huffman::classic
