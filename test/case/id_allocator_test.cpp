#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <set>

#include "libcotask/core/standard_int_id_allocator.h"
#include "frame/test_macros.h"


CASE_TEST(coroutine_task, id_allocator)
{
    cotask::core::standard_int_id_allocator<uint32_t> alloc;

    time_t begin_time = time(NULL);

    size_t id_num = 3 * (1 << 16) + 10000; //sleep 2-3 times
    std::set<uint32_t> s;

    for(size_t i = 0; i < id_num; ++ i)
    {
        uint32_t id = alloc.allocate();
        CASE_EXPECT_EQ(s.find(id), s.end());
        s.insert(id);
    }

    time_t end_time = time(NULL);
    CASE_EXPECT_EQ(id_num, s.size());
    CASE_EXPECT_GE(end_time - begin_time, (time_t)3);
    CASE_EXPECT_LT(end_time - begin_time, (time_t)4);
}
