@echo off

echo ###################### context coroutine (stack using memory pool) ################### >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_coroutines_mem_pool 10000 1000 200 >> "%2"
"%1/sample/Release/sample_stress_test_coroutines_mem_pool" 10000 1000 200 >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_coroutines_mem_pool 1000 1000 2048 >> "%2"
"%1/sample/Release/sample_stress_test_coroutines_mem_pool" 1000 1000 2048 >> "%2"
echo\  >> "%2"


echo ###################### context coroutine (stack using default allocator[VirtualAlloc]) ################### >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_coroutine 10000 1000 200 >> "%2"
"%1/sample/Release/sample_stress_test_coroutine" 10000 1000 200 >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_coroutine 1000 1000 2048 >> "%2"
"%1/sample/Release/sample_stress_test_coroutine" 1000 1000 2048 >> "%2"
echo\  >> "%2"


echo ###################### context coroutine (stack using malloc/free) ################### >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_coroutine_malloc 10000 1000 200 >> "%2"
"%1/sample/Release/sample_stress_test_coroutine_malloc" 10000 1000 200 >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_coroutine_malloc 1000 1000 2048 >> "%2"
"%1/sample/Release/sample_stress_test_coroutine_malloc" 1000 1000 2048 >> "%2"
echo\  >> "%2"


echo ###################### task (stack using memory pool) ################### >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_task_mem_pool 10000 1000 200 >> "%2"
"%1/sample/Release/sample_stress_test_task_mem_pool" 10000 1000 200 >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_task_mem_pool 1000 1000 2048 >> "%2"
"%1/sample/Release/sample_stress_test_task_mem_pool" 1000 1000 2048 >> "%2"
echo\  >> "%2"


echo ###################### task (stack using default allocator[VirtualAlloc]) ################### >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_task 10000 1000 200 >> "%2"
"%1/sample/Release/sample_stress_test_task" 10000 1000 200 >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_task 1000 1000 2048 >> "%2"
"%1/sample/Release/sample_stress_test_task" 1000 1000 2048 >> "%2"
echo\  >> "%2"


echo ###################### task (stack using malloc/free) ################### >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_task_malloc 10000 1000 200 >> "%2"
"%1/sample/Release/sample_stress_test_task_malloc" 10000 1000 200 >> "%2"
echo ########## Cmd: sample/Release/sample_stress_test_task_malloc 1000 1000 2048 >> "%2"
"%1/sample/Release/sample_stress_test_task_malloc" 1000 1000 2048 >> "%2"
echo\  >> "%2"



