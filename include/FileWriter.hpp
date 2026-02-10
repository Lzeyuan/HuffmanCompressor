// #ifndef FIELWIRTER_HPP_
// #define FIELWIRTER_HPP_

// #include <cstddef>
// #include <expected>
// #include <span>

// namespace leza::compression {

// class IWriter {
// public:
//   enum class ErrorCode { Finish };

//   virtual std::expected<int, ErrorCode>
//   write(std::span<std::byte> bytes) noexcept = 0;
// };

// class IReader {
// public:
//   enum class ErrorCode { EOF };

//   virtual std::expected<int, ErrorCode>
//   read(std::span<std::byte> bytes) noexcept = 0;
// };
// } // namespace leza::compression

// #endif // FIELWIRTER_HPP_