core
impl

#### action allocator
- alloc

#### task mem struct
+ id = uint64_t
* runner = libcotask::task_runner_base
+ runner_allocator = standard new allocator
+ coroutine container = libcopp::coroutine_context_default
+ - coroutine base type
+ - stack allocator

- _clear_action()
- _set_action(runner_base)
- _set_action(Functor)
- _set_action(function)
- _set_action(mem function)
- construct(action_base)
- construct(Functor)
- construct(function)
- construct(mem function)
- get_action()
- get_key()
- run()
- start()
- yield()
- cancel()
- get_status(): 任务状态 created | running | waiting | canceled | killed
- is_canceled(): 是否因取消而终止
- is_completed(): 是否因完成而终止
- is_faulted(): 是否因未处理错误而终止
- is_killed(): 是否是被强制终止
- next(task)
- wait(task)
- wait_any(task*, size) : 依据 process_id_
- wait_all(task*, size) : 拓扑排序, 依据 process_id_
- on_finished() = 0


#### entry(task) container = standard stl map container
+ insert()
+ find()
+ remove()
+ size()

#### entry(task) manager
+ task
+ object_allocator = standard new allocator
+ task container

- set_id_allocator() = standard int allocator
- set_runner_allocator() = 
- scheduling_once()
- scheduling_loop()
- add_scheduler()




