#!/bin/bash

SYS_NAME="$(uname -s)";
SYS_NAME="$(basename $SYS_NAME)";
CC=gcc;
CXX=g++;
CCACHE="$(which ccache)";

CMAKE_OPTIONS="-DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON";
CMAKE_CLANG_TIDY="";
CMAKE_CLANG_ANALYZER=0;
CMAKE_CLANG_ANALYZER_PATH="";
BUILD_DIR=$(echo "build_$SYS_NAME" | tr '[:upper:]' '[:lower:]');
CPPCHECK_PLATFORM=unix64;

if [ ! -z "$MSYSTEM" ]; then
    CHECK_MSYS=$(echo "${MSYSTEM:0:5}" | tr '[:upper:]' '[:lower:]');
else
    CHECK_MSYS="";
fi

while getopts "ac:e:htusp:-" OPTION; do
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
            echo "-p                            cppcheck platform(defult: $CPPCHECK_PLATFORM).";
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
        p)
            CPPCHECK_PLATFORM="$OPTARG";
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

if [ ! -z "$CC" ]; then
    CCNAME="${CC##*/}";
    CCNAME="${CCNAME##*\\}";
    BUILD_DIR="${BUILD_DIR}_$CCNAME";
fi

SCRIPT_DIR="$(cd $(dirname $0) && pwd)";
mkdir -p "$SCRIPT_DIR/$BUILD_DIR";
cd "$SCRIPT_DIR/$BUILD_DIR";

if [ "${CC:0-8}" == "cppcheck" ]; then
    echo "Run cppcheck in $SCRIPT_DIR";
    $CC --enable=all --xml --inconclusive --platform=$CPPCHECK_PLATFORM --xml-version=2 \
        --language=c++ --std=c++11 --suppress=missingIncludeSystem --max-configs=256 \
        --relative-paths=$SCRIPT_DIR -I $SCRIPT_DIR/include -I $SCRIPT_DIR/test $SCRIPT_DIR/src $SCRIPT_DIR/test $SCRIPT_DIR/sample 2>libcopp.cppcheck.xml ;
    CPPCHECK_REPORTHTML="$(which cppcheck-htmlreport 2>/dev/null)";
    if [ ! -z "$CPPCHECK_REPORTHTML" ]; then
        echo "====== Try to generate html report. ======";
        $CPPCHECK_REPORTHTML --file=libcopp.cppcheck.xml --report-dir=libcopp.cppcheck.html --source-dir=$SCRIPT_DIR ;
    fi
    exit 0;
fi

if [ ! -z "$CCACHE" ] && [ "$CCACHE" != "disable" ] && [ "$CCACHE" != "disabled" ] && [ "$CCACHE" != "no" ] && [ "$CCACHE" != "false" ] && [ -e "$CCACHE" ]; then
    #CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$CCACHE -DCMAKE_CXX_COMPILER=$CCACHE -DCMAKE_C_COMPILER_ARG1=$CC -DCMAKE_CXX_COMPILER_ARG1=$CXX";
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER_LAUNCHER=$CCACHE -DCMAKE_CXX_COMPILER_LAUNCHER=$CCACHE -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
elif [ ! -z "$CC" ]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX";
fi

if [ "$CHECK_MSYS" == "mingw" ] && [ ! -z "$CC" ] ; then
    cmake .. -G "MSYS Makefiles" $CMAKE_OPTIONS "$@";
else
    cmake .. $CMAKE_OPTIONS "$@";
fi


if [ 1 -eq $CMAKE_CLANG_ANALYZER ]; then
    echo "=========================================================================================================";
    CMAKE_CLANG_ANALYZER_OPTIONS="";
    if [ -e "$SCRIPT_DIR/.scan-build.enable" ]; then
        for OPT in $(cat "$SCRIPT_DIR/.scan-build.enable"); do
            CMAKE_CLANG_ANALYZER_OPTIONS="$CMAKE_CLANG_ANALYZER_OPTIONS -enable-checker $OPT";
        done
    fi

    if [ -e "$SCRIPT_DIR/.scan-build.disable" ]; then
        for OPT in $(cat "$SCRIPT_DIR/.scan-build.disable"); do
            CMAKE_CLANG_ANALYZER_OPTIONS="$CMAKE_CLANG_ANALYZER_OPTIONS -disable-checker $OPT";
        done
    fi

    if [ -z "$CMAKE_CLANG_ANALYZER_PATH" ]; then
        echo "cd '$SCRIPT_DIR/$BUILD_DIR' && scan-build -o report --html-title='libcopp static analysis' $CMAKE_CLANG_ANALYZER_OPTIONS make -j4";
    else
        echo "cd '$SCRIPT_DIR/$BUILD_DIR' && env PATH=\"\$PATH:$CMAKE_CLANG_ANALYZER_PATH\" scan-build -o report --html-title='libmt_core static analysis' $CMAKE_CLANG_ANALYZER_OPTIONS make -j4";
    fi
    echo "Now, you can run those code above to get a static analysis report";
    echo "You can get help and binary of clang-analyzer and scan-build at http://clang-analyzer.llvm.org/scan-build.html"
fi

