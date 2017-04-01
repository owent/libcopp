@echo off

"%1/sample/Release/sample_benchmark_coroutine_mem_pool.exe" 10000 1000 200 >> "%2"
"%1/sample/Release/sample_benchmark_coroutine_mem_pool.exe" 1000 1000 2048 >> "%2"
echo\  >> "%2"

"%1/sample/Release/sample_benchmark_coroutine.exe" 10000 1000 200 >> "%2"
"%1/sample/Release/sample_benchmark_coroutine.exe" 1000 1000 2048 >> "%2"
echo\  >> "%2"

"%1/sample/Release/sample_benchmark_coroutine_malloc.exe" 10000 1000 200 >> "%2"
"%1/sample/Release/sample_benchmark_coroutine_malloc.exe" 1000 1000 2048 >> "%2"
echo\  >> "%2"

"%1/sample/Release/sample_benchmark_task_mem_pool.exe" 10000 1000 200 >> "%2"
"%1/sample/Release/sample_benchmark_task_mem_pool.exe" 1000 1000 2048 >> "%2"
echo\  >> "%2"

"%1/sample/Release/sample_benchmark_task.exe" 10000 1000 200 >> "%2"
"%1/sample/Release/sample_benchmark_task.exe" 1000 1000 2048 >> "%2"
echo\  >> "%2"

"%1/sample/Release/sample_benchmark_task_malloc.exe" 10000 1000 200 >> "%2"
"%1/sample/Release/sample_benchmark_task_malloc.exe" 1000 1000 2048 >> "%2"
echo\  >> "%2"

