#!/bin/bash

SYS_NAME="$(uname -s)";
SYS_NAME="$(basename $SYS_NAME)";
CC=gcc;
CXX=g++;
CCACHE="$(which ccache)";
DISTCC="";
if [[ ! -z "$DISTCC_HOSTS" ]]; then
    DISTCC="$(which distcc 2>/dev/null)";
fi

NINJA_BIN="$(which ninja 2>&1)";
if [[ $? -ne 0 ]]; then
    NINJA_BIN="$(which ninja-build 2>&1)";
    if [[ $? -ne 0 ]]; then
        NINJA_BIN="";
    fi
fi

CMAKE_OPTIONS="";
CMAKE_CLANG_TIDY="";
CMAKE_CLANG_ANALYZER=0;
CMAKE_CLANG_ANALYZER_PATH="";
BUILD_DIR=$(echo "build_jobs_$SYS_NAME" | tr '[:upper:]' '[:lower:]');
CUSTOM_BUILD_DIR=;
CMAKE_BUILD_TYPE=Debug;

if [[ ! -z "$MSYSTEM" ]]; then
    CHECK_MSYS=$(echo "${MSYSTEM:0:5}" | tr '[:upper:]' '[:lower:]');
else
    CHECK_MSYS="";
fi

while getopts "ab:c:d:e:hlr:tus-" OPTION; do
    case $OPTION in
        a)
            echo "Ready to check ccc-analyzer and c++-analyzer, please do not use -c to change the compiler when using clang-analyzer.";
            CC=$(which ccc-analyzer);
            CXX=$(which c++-analyzer);
            if [ 0 -ne $? ]; then
                # check mingw path
                if [ "mingw" == "$CHECK_MSYS" ]; then
                    if [ ! -z "$MINGW_MOUNT_POINT" ] && [ -e "$MINGW_MOUNT_POINT/libexec/ccc-analyzer.bat" ] && [ -e "$MINGW_MOUNT_POINT/libexec/ccc-analyzer.bat" ]; then
                        echo "clang-analyzer found in $MINGW_MOUNT_POINT";
                        export PATH=$PATH:$MINGW_MOUNT_POINT/libexec ;
                        CC="$MINGW_MOUNT_POINT/libexec/ccc-analyzer.bat";
                        CXX="$MINGW_MOUNT_POINT/libexec/ccc-analyzer.bat";
                        CMAKE_CLANG_ANALYZER_PATH="$MINGW_MOUNT_POINT/libexec";
                    elif [ ! -z "$MINGW_PREFIX" ] && [ -e "$MINGW_PREFIX/libexec/ccc-analyzer.bat" ] && [ -e "$MINGW_PREFIX/libexec/c++-analyzer.bat" ]; then
                        echo "clang-analyzer found in $MINGW_PREFIX";
                        export PATH=$PATH:$MINGW_PREFIX/libexec ;
                        CC="$MINGW_PREFIX/libexec/ccc-analyzer.bat";
                        CXX="$MINGW_PREFIX/libexec/ccc-analyzer.bat";
                        CMAKE_CLANG_ANALYZER_PATH="$MINGW_PREFIX/libexec";
                    fi
                fi
            fi

            if [ -z "$CC" ] || [ -z "$CXX" ]; then
                echo "ccc-analyzer=$CC";
                echo "c++-analyzer=$CXX";
                echo "clang-analyzer not found, failed.";
                exit 1;
            fi
            echo "ccc-analyzer=$CC";
            echo "c++-analyzer=$CXX";
            echo "clang-analyzer setup completed.";
            CMAKE_CLANG_ANALYZER=1;
            BUILD_DIR="${BUILD_DIR}_analyzer";
        ;;
        b)
            CMAKE_BUILD_TYPE="$OPTARG";
        ;;
        c)
            if [[ $CMAKE_CLANG_ANALYZER -ne 0 ]]; then
                CCC_CC="$OPTARG";
                CCC_CXX="${CCC_CC/%clang/clang++}";
                CCC_CXX="${CCC_CXX/%gcc/g++}";
                export CCC_CC;
                export CCC_CXX;
            else
                CC="$OPTARG";
                CXX="$(echo "$CC" | sed 's/\(.*\)clang/\1clang++/')";
                CXX="$(echo "$CXX" | sed 's/\(.*\)gcc/\1g++/')";
            fi
        ;;
        d)
            DISTCC="$OPTARG";
        ;;
        e)
            CCACHE="$OPTARG";
        ;;
        h)
            echo "usage: $0 [options] [-- [cmake options...] ]";
            echo "options:";
            echo "-a                            using clang-analyzer.";
            echo "-b <build type>               set build type(Debug, RelWithDebINfo, Release, MinSizeRel).";
            echo "-c <compiler>                 compiler toolchains(gcc, clang or others).";
            echo "-d [libsodium root]           set root of libsodium.";
            echo "-e <ccache path>              try to use specify ccache to speed up building.";
            echo "-h                            help message.";
            echo "-m [mbedtls root]             set root of mbedtls.";
            echo "-o [openssl root]             set root of openssl.";
            echo "-t                            enable clang-tidy.";
            echo "-u                            enable unit test.";
            echo "-s                            enable sample.";
            exit 0;
        ;;
        l)
            CMAKE_OPTIONS="$CMAKE_OPTIONS -DPROJECT_ENABLE_TOOLS=YES";
        ;;
        r)
            CUSTOM_BUILD_DIR="$OPTARG";
        ;;
        t)
            CMAKE_CLANG_TIDY="-D -checks=* --";
        ;;
        u)
            CMAKE_OPTIONS="$CMAKE_OPTIONS -DPROJECT_ENABLE_UNITTEST=YES";
        ;;
        s)
            CMAKE_OPTIONS="$CMAKE_OPTIONS -DPROJECT_ENABLE_SAMPLE=YES";
        ;;
        -)
            break;
        ;;
        ?)
            echo "unkonw argument detected";
            exit 1;
        ;;
    esac
done

shift $(($OPTIND - 1));
SCRIPT_DIR="$(cd $(dirname $0) && pwd)";

if [[ "x$CUSTOM_BUILD_DIR" != "x" ]]; then
    BUILD_DIR="$CUSTOM_BUILD_DIR";
fi

mkdir -p "$SCRIPT_DIR/$BUILD_DIR";
cd "$SCRIPT_DIR/$BUILD_DIR";

if [[ ! -z "$DISTCC" ]] && [[ "$DISTCC" != "disable" ]] && [[ "$DISTCC" != "disabled" ]] && [[ "$DISTCC" != "no" ]] && [[ "$DISTCC" != "false" ]] && [[ -e "$DISTCC" ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER_LAUNCHER=$DISTCC -DCMAKE_CXX_COMPILER_LAUNCHER=$DISTCC -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
elif [[ ! -z "$CCACHE" ]] && [[ "$CCACHE" != "disable" ]] && [[ "$CCACHE" != "disabled" ]] && [[ "$CCACHE" != "no" ]] && [[ "$CCACHE" != "false" ]] && [[ -e "$CCACHE" ]]; then
    #CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$CCACHE -DCMAKE_CXX_COMPILER=$CCACHE -DCMAKE_C_COMPILER_ARG1=$CC -DCMAKE_CXX_COMPILER_ARG1=$CXX";
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER_LAUNCHER=$CCACHE -DCMAKE_CXX_COMPILER_LAUNCHER=$CCACHE -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
fi

if [[ "x$NINJA_BIN" != "x" ]]; then
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE $CMAKE_OPTIONS "$@";
elif [[ "$CHECK_MSYS" == "mingw" ]]; then
    cmake --help | grep '^[[:space:]]*MinGW Makefiles' > /dev/null 2>&1 ;
    if [[ $? -eq 0 ]]; then
        cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE $CMAKE_OPTIONS "$@";
    else
        cmake .. -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE $CMAKE_OPTIONS "$@";
    fi
elif [[ "$CHECK_MSYS" == "msys" ]]; then
    cmake .. -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE $CMAKE_OPTIONS "$@";
else
    cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE $CMAKE_OPTIONS "$@";
fi


if [[ 1 -eq $CMAKE_CLANG_ANALYZER ]]; then
    echo "=========================================================================================================";
    CMAKE_CLANG_ANALYZER_OPTIONS="";
    if [[ -e "$SCRIPT_DIR/.scan-build.enable" ]]; then
        for OPT in $(cat "$SCRIPT_DIR/.scan-build.enable"); do
            CMAKE_CLANG_ANALYZER_OPTIONS="$CMAKE_CLANG_ANALYZER_OPTIONS -enable-checker $OPT";
        done
    fi

    if [[ -e "$SCRIPT_DIR/.scan-build.disable" ]]; then
        for OPT in $(cat "$SCRIPT_DIR/.scan-build.disable"); do
            CMAKE_CLANG_ANALYZER_OPTIONS="$CMAKE_CLANG_ANALYZER_OPTIONS -disable-checker $OPT";
        done
    fi

    REPORT_TITLE="$(basename $(git remote get-url $(git remote | head -n 1)))";
    REPORT_TITLE="${REPORT_TITLE%*.git}";

    if [[ -z "$CMAKE_CLANG_ANALYZER_PATH" ]]; then
        echo "cd '$SCRIPT_DIR/$BUILD_DIR' && scan-build -o report --html-title='$REPORT_TITLE static analysis' $CMAKE_CLANG_ANALYZER_OPTIONS make -j4";
    else
        echo "cd '$SCRIPT_DIR/$BUILD_DIR' && env PATH=\"\$PATH:$CMAKE_CLANG_ANALYZER_PATH\" scan-build -o report --html-title='$REPORT_TITLE static analysis' $CMAKE_CLANG_ANALYZER_OPTIONS make -j4";
    fi
    echo "Now, you can run those code above to get a static analysis report";
    echo "You can get help and binary of clang-analyzer and scan-build at http://clang-analyzer.llvm.org/scan-build.html"
fi
