# ===== gtest =====
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  GIT_SHALLOW TRUE
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
  BinaryIOTest
  BinaryIOTest.cpp
)
target_link_libraries(
  BinaryIOTest
  GTest::gtest_main
  HuffmanCompressorLib
)

include(GoogleTest)
gtest_discover_tests(HelloTest)
gtest_discover_tests(BinaryIOTest)
