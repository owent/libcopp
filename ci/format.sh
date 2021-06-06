#!/bin/bash

find . -type f                                        \
  -regex "^./third_party/packages/.*" -prune          \
  -o -regex "^./third_party/install/.*" -prune        \
  -o -regex "^./atframework/cmake-toolset/.*" -prune  \
  -o -regex "^./build_jobs_.*" -prune                 \
  -o -regex "^./build" -prune                         \
  -o -name "*.cmake" -print                           \
  -o -name "*.cmake.in" -print                        \
  -o -name 'CMakeLists.txt' -print                    \
  | xargs cmake-format -i

find . -type f                                        \
  -regex "^./third_party/packages/.*" -prune          \
  -o -regex "^./third_party/install/.*" -prune        \
  -o -regex "^./atframework/cmake-toolset/.*" -prune  \
  -o -regex "^./build_jobs_.*" -prune                 \
  -o -regex "^./build" -prune                         \
  -o -name "*.h" -print                               \
  -o -name "*.hpp" -print                             \
  -o -name "*.cxx" -print                             \
  -o -name '*.cpp' -print                             \
  -o -name '*.cc' -print                              \
  -o -name '*.c' -print                               \
  | xargs -r -n 32 clang-format -i --style=file --fallback-style=none
