#ifndef NONCOPYABLE_HPP_
#define NONCOPYABLE_HPP_

namespace leza::utility {
class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;

protected:
  NonCopyable() = default;
  ~NonCopyable() = default;
};

class MoveOnly {
public:
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly &operator=(const MoveOnly &) = delete;

  MoveOnly(MoveOnly &&) = default;
  MoveOnly &operator=(MoveOnly &&) = default;

protected:
  MoveOnly() = default;
  ~MoveOnly() = default;
};

} // namespace leza::utility
#endif // NONCOPYABLE_HPP_