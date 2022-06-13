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
file(GLOB TEMP_WEB_SERVER_SRC "src/*/*.cpp")

include(GoogleTest)

file(GLOB TEST_FILE "test/*/*.cpp")
# 遍历所有测试文件夹
foreach(test_file ${TEST_FILE})
  # 获取无后缀文件名作为编译目标
  get_filename_component(target ${test_file} NAME_WE)

  # 编译测试文件
  add_executable(
    ${target}
    ${test_file}
    ${TEMP_WEB_SERVER_SRC}
  )

  # 链接测试文件
  # 链接mysqlclient
  target_link_libraries(
    ${target}
    gtest_main
    mysqlclient
  )

  gtest_discover_tests(${target})
endforeach(test_file)