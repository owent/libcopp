#!/bin/bash

SYS_NAME="$(uname -s)";
SYS_NAME="$(basename $SYS_NAME)";
CC=gcc;
CXX=g++;
CCACHE="$(which ccache)";

CMAKE_OPTIONS="";
CMAKE_CLANG_TIDY="";
CMAKE_CLANG_ANALYZER=0;
CMAKE_CLANG_ANALYZER_PATH="";
BUILD_DIR=$(echo "build_$SYS_NAME" | tr '[:upper:]' '[:lower:]');

if [ ! -z "$MSYSTEM" ]; then
    CHECK_MSYS=$(echo "${MSYSTEM:0:5}" | tr '[:upper:]' '[:lower:]');
else
    CHECK_MSYS="";
fi

while getopts "ac:e:hltus-" OPTION; do
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
        c)
            CC="$OPTARG";
            CXX="${CC/clang/clang++}";
            CXX="${CXX/gcc/g++}";
        ;;
        e)
            CCACHE="$OPTARG";
        ;;
        h)
            echo "usage: $0 [options] [-- [cmake options...] ]";
            echo "options:";
            echo "-a                            using clang-analyzer.";
            echo "-c <compiler>                 compiler toolchains(gcc, clang or others).";
            echo "-e <ccache path>              try to use specify ccache to speed up building.";
            echo "-h                            help message.";
            echo "-t                            enable clang-tidy.";
            echo "-u                            enable unit test.";
            echo "-s                            enable sample.";
            echo "-l                            enable tools.";
            exit 0;
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
        l)
            CMAKE_OPTIONS="$CMAKE_OPTIONS -DPROJECT_ENABLE_TOOLS=YES";
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
SCRIPT_DIR="$(dirname $0)";
mkdir -p "$SCRIPT_DIR/$BUILD_DIR";
cd "$SCRIPT_DIR/$BUILD_DIR";

if [ ! -z "$CCACHE" ] && [ "$CCACHE" != "disable" ] && [ "$CCACHE" != "disabled" ] && [ "$CCACHE" != "no" ] && [ "$CCACHE" != "false" ] && [ -e "$CCACHE" ]; then
    #CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$CCACHE -DCMAKE_CXX_COMPILER=$CCACHE -DCMAKE_C_COMPILER_ARG1=$CC -DCMAKE_CXX_COMPILER_ARG1=$CXX";
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER_LAUNCHER=$CCACHE -DCMAKE_CXX_COMPILER_LAUNCHER=$CCACHE -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
fi

if [ "$CHECK_MSYS" == "mingw" ]; then
    cmake .. -G "MSYS Makefiles" $CMAKE_OPTIONS "$@";
else
    cmake .. $CMAKE_OPTIONS "$@";
fi


if [ 1 -eq $CMAKE_CLANG_ANALYZER ]; then
    echo "=========================================================================================================";
    if [ -z "$CMAKE_CLANG_ANALYZER_PATH" ]; then
        echo "cd '$SCRIPT_DIR/$BUILD_DIR' && scan-build -o report --html-title='atsf4g-co static analysis' make -j4";
    else
        echo "cd '$SCRIPT_DIR/$BUILD_DIR' && env PATH=\"\$PATH:$CMAKE_CLANG_ANALYZER_PATH\" scan-build -o report --html-title='atsf4g-co static analysis' make -j4";
    fi
    echo "Now, you can run those code above to get a static analysis report";
    echo "You can get help and binary of clang-analyzer and scan-build at http://clang-analyzer.llvm.org/scan-build.html"
fi
