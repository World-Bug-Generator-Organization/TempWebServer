cmake_minimum_required(VERSION 3.14)
project(TempWebServer CXX)

# GoogleTest 需要至少 C++11
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# GoogleTest基础设置
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# 添加子目录和头文件
include_directories("src")
aux_source_directory("src/log" LOG_SRC)

add_executable(
  block_queue_test
  test/log/block_queue_test.cpp
  ${LOG_SRC}
)

target_link_libraries(
  block_queue_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(block_queue_test)
