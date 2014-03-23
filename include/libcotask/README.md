core
impl

#### task mem struct
+ id = uint64_t
+ runner = libcotask::task_runner_base
+ coroutine container = libcopp::coroutine_context_default
+ - coroutine base type
+ - stack allocator

- _clear_runner()
- set_runner()
- get_key()
- 

#### task container = standard stl map container
+ insert()
+ find()
+ remove()
+ size()

#### task manager
+ task
+ runner_allocator = standard new allocator
+ task container

- set_id_allocator() = standard int allocator
- set_runner_allocator() = 


