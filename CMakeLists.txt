cmake_minimum_required(VERSION 3.15)
project(RandomGen VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(RandomGen STATIC
  src/RandomGen.cpp
)
target_include_directories(RandomGen PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_executable(RandomGenExample examples/RandomGenExample.cpp)
target_link_libraries(RandomGenExample PRIVATE RandomGen)

option(BUILD_TESTS "Build the tests" ON)
if(BUILD_TESTS)
  if(NOT TARGET gtest)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.12.1
      UPDATE_DISCONNECTED ON
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()

  enable_testing()
  include(GoogleTest)

  add_executable(RandomGenTests tests/RandomGenTests.cpp)
  target_link_libraries(RandomGenTests PRIVATE 
    RandomGen
    gtest
    gtest_main
    gmock
    gmock_main
  )
  
  gtest_discover_tests(RandomGenTests)
endif()

install(TARGETS RandomGen
  EXPORT RandomGenTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)