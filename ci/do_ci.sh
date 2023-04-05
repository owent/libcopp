#!/bin/bash

cd "$(cd "$(dirname $0)" && pwd)/.."

set -ex

if [[ -z "$CONFIGURATION" ]]; then
  CONFIGURATION=RelWithDebInfo
fi

if [[ "x$USE_CC" == "xclang-latest" ]]; then
  echo '#include <iostream>
  int main() { std::cout<<"Hello"; }' >test-libc++.cpp
  SELECT_CLANG_VERSION=""
  SELECT_CLANG_HAS_LIBCXX=1
  clang -x c++ -stdlib=libc++ test-libc++.cpp -lc++ -lc++abi || SELECT_CLANG_HAS_LIBCXX=0
  if [[ $SELECT_CLANG_HAS_LIBCXX -eq 0 ]]; then
    CURRENT_CLANG_VERSION=$(clang -x c /dev/null -dM -E | grep __clang_major__ | awk '{print $NF}')
    for ((i = $CURRENT_CLANG_VERSION + 5; $i >= $CURRENT_CLANG_VERSION; --i)); do
      SELECT_CLANG_HAS_LIBCXX=1
      SELECT_CLANG_VERSION="-$i"
      clang$SELECT_CLANG_VERSION -x c++ -stdlib=libc++ test-libc++.cpp -lc++ -lc++abi || SELECT_CLANG_HAS_LIBCXX=0
      if [[ $SELECT_CLANG_HAS_LIBCXX -eq 1 ]]; then
        break
      fi
    done
  fi
  SELECT_CLANGPP_BIN=clang++$SELECT_CLANG_VERSION
  LINK_CLANGPP_BIN=0
  which $SELECT_CLANGPP_BIN || LINK_CLANGPP_BIN=1
  if [[ $LINK_CLANGPP_BIN -eq 1 ]]; then
    mkdir -p .local/bin
    ln -s "$(which "clang$SELECT_CLANG_VERSION")" "$PWD/.local/bin/clang++$SELECT_CLANG_VERSION"
    export PATH="$PWD/.local/bin:$PATH"
  fi
  export USE_CC=clang$SELECT_CLANG_VERSION
elif [[ "x$USE_CC" == "xgcc-latest" ]]; then
  CURRENT_GCC_VERSION=$(gcc -x c /dev/null -dM -E | grep __GNUC__ | awk '{print $NF}')
  echo '#include <iostream>
  int main() { std::cout<<"Hello"; }' >test-gcc-version.cpp
  let LAST_GCC_VERSION=$CURRENT_GCC_VERSION+10
  for ((i = $CURRENT_GCC_VERSION; $i <= $LAST_GCC_VERSION; ++i)); do
    TEST_GCC_VERSION=1
    g++-$i -x c++ test-gcc-version.cpp || TEST_GCC_VERSION=0
    if [[ $TEST_GCC_VERSION -eq 0 ]]; then
      break
    fi
    CURRENT_GCC_VERSION=$i
  done
  export USE_CC=gcc-$CURRENT_GCC_VERSION
  echo "Using $USE_CC"
fi

PROJECT_ADDON_OPTIONS=("-DLIBCOPP_FCONTEXT_USE_TSX=ON" "-DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON"
  "-DPROJECT_ENABLE_TOOLS=ON" "-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_LOW_MEMORY_MODE=ON")

if [[ "x$BUILD_SHARED_LIBS" != "x" ]]; then
  PROJECT_ADDON_OPTIONS=(${PROJECT_ADDON_OPTIONS[@]} "-DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS")
fi

if [[ "x$NO_EXCEPTION" != "x" ]]; then
  PROJECT_ADDON_OPTIONS=(${PROJECT_ADDON_OPTIONS[@]} "-DCMAKE_CXX_FLAGS=-fno-exceptions")
fi
if [[ "x$THREAD_UNSAFE" != "x" ]]; then
  PROJECT_ADDON_OPTIONS=(${PROJECT_ADDON_OPTIONS[@]} "-DLIBCOPP_DISABLE_ATOMIC_LOCK=ON" "-DLIBCOPP_LOCK_DISABLE_THIS_MT=ON")
fi

if [[ "$1" == "format" ]]; then
  python3 -m pip install --user -r ./ci/requirements.txt
  export PATH="$HOME/.local/bin:$PATH"
  bash ./ci/format.sh
  CHANGED="$(git -c core.autocrlf=true ls-files --modified)"
  if [[ ! -z "$CHANGED" ]]; then
    echo "The following files have changes:"
    echo "$CHANGED"
    git diff
    # exit 1 ; # Just warning, some versions of clang-format have different default style for unsupport syntax
  fi
  exit 0
elif [[ "$1" == "coverage" ]]; then
  CONFIGURATION=Debug
  bash cmake_dev.sh -lus -b $CONFIGURATION -r build_jobs_coverage -c $USE_CC -- "-DCMAKE_C_FLAGS=$GCOV_FLAGS" \
    "-DCMAKE_CXX_FLAGS=$GCOV_FLAGS" "-DCMAKE_EXE_LINKER_FLAGS=$GCOV_FLAGS" ${PROJECT_ADDON_OPTIONS[@]}
  cd build_jobs_coverage
  cmake --build . -j2 --config $CONFIGURATION || cmake --build . --config $CONFIGURATION
elif [[ "$1" == "codeql.configure" ]]; then
  bash cmake_dev.sh -l -b $CONFIGURATION -r build_jobs_ci -c $USE_CC -- "-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_LOW_MEMORY_MODE=ON"
elif [[ "$1" == "codeql.build" ]]; then
  cd build_jobs_ci
  cmake --build . -j --config $CONFIGURATION || cmake --build . --config $CONFIGURATION
elif [[ "$1" == "gcc.test" ]] || [[ "$1" == "gcc.legacy.test" ]] || [[ "$1" == "clang.test" ]] || [[ "$1" == "memcheck" ]]; then
  bash cmake_dev.sh -lus -b $CONFIGURATION -r build_jobs_ci -c $USE_CC -- ${PROJECT_ADDON_OPTIONS[@]} "-DCMAKE_INSTALL_PREFIX=$PWD/build_jobs_ci/prebuilt"
  cd build_jobs_ci
  cmake --build . -j2 --config $CONFIGURATION || cmake --build . --config $CONFIGURATION
  cmake --install .
  ls -lh $PWD/build_jobs_ci/prebuilt/include/libcopp/utils/config/libcopp_build_features.h
elif [[ "$1" == "msys2.mingw.test" ]]; then
  pacman -S --needed --noconfirm mingw-w64-x86_64-cmake git m4 curl wget tar autoconf automake \
    mingw-w64-x86_64-git-lfs mingw-w64-x86_64-toolchain mingw-w64-x86_64-libtool \
    mingw-w64-x86_64-python mingw-w64-x86_64-python-pip mingw-w64-x86_64-python-setuptools || true
  git config --global http.sslBackend openssl
  mkdir -p build_jobs_ci
  cd build_jobs_ci
  cmake .. -G 'MinGW Makefiles' -DCMAKE_BUILD_TYPE=$CONFIGURATION ${PROJECT_ADDON_OPTIONS[@]}
  cmake --build . -j2 --config $CONFIGURATION || cmake --build . --config $CONFIGURATION
elif [[ "$1" == "document" ]]; then
  mkdir -p build_for_doxygen
  cd build_for_doxygen
  export PATH="$HOME/.local/bin:$PATH"
  cmake .. -DCMAKE_BUILD_TYPE=$CONFIGURATION -DLIBCOPP_FCONTEXT_USE_TSX=ON -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON
  cd ../docs
  python3 -m pip install --user --upgrade pip
  python3 -m pip install --user --upgrade -r requirements.txt
  mkdir -p sphinx/doxygen
  doxygen libcopp.doxyfile
  du -sh sphinx/doxygen/*
  sphinx-build -b html -a -D breathe_projects.libcopp=doxygen/xml sphinx output
  if [[ -e "output/doxygen/html" ]]; then
    rm -rf "output/doxygen/html"
  fi
  mkdir -p output/doxygen/
  mv -f "sphinx/doxygen/html" "output/doxygen/html"
  echo "libcopp.atframe.work" >output/CNAME
  exit 0
else
  echo "Bad configure"
  exit 1
fi

if [[ "$1" == "memcheck" ]]; then
  echo "============================== run valgrind memcheck =============================="
  ctest -VV . -C $CONFIGURATION -L libcopp.memcheck
  for MEMCHECK_REPORT in $(find . -maxdepth 5 -name "*.memcheck.log"); do
    echo "------------------------------ valgrind memcheck : $MEMCHECK_REPORT ------------------------------"
    cat "$MEMCHECK_REPORT"
  done
elif [[ "x$THREAD_UNSAFE" != "x" ]]; then
  echo "============================== run benchmark =============================="
  ctest -VV . -C $CONFIGURATION -L libcopp.benchmark
else
  echo "============================== run sample,test,benchmark =============================="
  ctest -VV . -C $CONFIGURATION -L libcopp.sample
  ctest -VV . -C $CONFIGURATION -L libcopp.unit_test
  ctest -VV . -C $CONFIGURATION -L libcopp.benchmark
fi

if [[ "$1" == "coverage" ]]; then
  lcov --directory "$PWD" --capture --output-file coverage.info
fi
