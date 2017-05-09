#!/bin/sh

if [ $# -lt 2 ]; then
    echo "usage: $0 <build dir> <report file>";
    exit -1;
fi

"$1"/sample/sample_benchmark_coroutine_mem_pool 10000 1000 200 >> "$2";
"$1"/sample/sample_benchmark_coroutine_mem_pool 1000 1000 2048 >> "$2";
echo "" >> "$2";

"$1"/sample/sample_benchmark_coroutine 10000 1000 200 >> "$2";
"$1"/sample/sample_benchmark_coroutine 1000 1000 2048 >> "$2";
echo "" >> "$2";

"$1"/sample/sample_benchmark_coroutine_malloc 10000 1000 200 >> "$2";
"$1"/sample/sample_benchmark_coroutine_malloc 1000 1000 2048 >> "$2";
echo "" >> "$2";

"$1"/sample/sample_benchmark_task_mem_pool 10000 1000 200 >> "$2";
"$1"/sample/sample_benchmark_task_mem_pool 1000 1000 2048 >> "$2";
echo "" >> "$2";

"$1"/sample/sample_benchmark_task 10000 1000 200 >> "$2";
"$1"/sample/sample_benchmark_task 1000 1000 2048 >> "$2";
echo "" >> "$2";

"$1"/sample/sample_benchmark_task_malloc 10000 1000 200 >> "$2";
"$1"/sample/sample_benchmark_task_malloc 1000 1000 2048 >> "$2";
echo "" >> "$2";



