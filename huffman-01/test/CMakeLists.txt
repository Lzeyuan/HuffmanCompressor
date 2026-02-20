# ===== gtest =====
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0  # 使用具体的发布版本标签
  GIT_SHALLOW TRUE  # 可选：只克隆最新提交，减少下载量
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# ===== gtest =====

enable_testing()

add_executable(
  HelloTest
  HelloTest.cpp
)
target_link_libraries(
  HelloTest
  GTest::gtest_main
)

add_executable(
  HuffmanCompressorTest
  HuffmanCompressorTest.cpp
)
target_link_libraries(
  HuffmanCompressorTest
  HuffmanCompressorLib
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(HelloTest)
gtest_discover_tests(HuffmanCompressorTest)
