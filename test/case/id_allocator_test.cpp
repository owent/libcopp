#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <set>


#include "frame/test_macros.h"
#include "libcotask/core/standard_int_id_allocator.h"


#if ((defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)) && \
    defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS

#include <memory>
#include <thread>

#endif

CASE_TEST(coroutine_task, id_allocator_16) {
    cotask::core::standard_int_id_allocator<uint16_t> alloc;

    time_t begin_time = time(NULL);

    size_t id_num = 3 * (1 << 8) + 100; // sleep 2-3 times
    std::set<uint16_t> s;

    for (size_t i = 0; i < id_num; ++i) {
        uint16_t id = alloc.allocate();
        CASE_EXPECT_TRUE(s.find(id) == s.end());
        s.insert(id);
    }

    time_t end_time = time(NULL);
    CASE_EXPECT_EQ(id_num, s.size());
    CASE_EXPECT_GE(end_time - begin_time, (time_t)2);
}

CASE_TEST(coroutine_task, id_allocator_32) {
    cotask::core::standard_int_id_allocator<uint32_t> alloc;

    time_t begin_time = time(NULL);

    size_t id_num = 3 * (1 << 16) + 100; // sleep 2-3 times
    std::set<uint32_t> s;

    for (size_t i = 0; i < id_num; ++i) {
        uint32_t id = alloc.allocate();
        CASE_EXPECT_TRUE(s.find(id) == s.end());
        s.insert(id);
    }

    time_t end_time = time(NULL);
    CASE_EXPECT_EQ(id_num, s.size());
    CASE_EXPECT_GE(end_time - begin_time, (time_t)2);
}


#if ((defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)) && \
    defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS

CASE_TEST(coroutine_task, id_allocator_mt) {
    cotask::core::standard_int_id_allocator<uint32_t> alloc;

    time_t begin_time = time(NULL);
    std::unique_ptr<std::thread> thds[4];
    std::set<uint32_t> s[4];
    for (int i = 0; i < 4; ++i) {
        std::set<uint32_t> *sp = &s[i];
        thds[i].reset(new std::thread([sp, &alloc]() {
            size_t id_num = 36768;

            for (size_t i = 0; i < id_num; ++i) {
                uint32_t id = alloc.allocate();
                CASE_EXPECT_TRUE(sp->find(id) == sp->end());
                sp->insert(id);
            }
        }));
    }

    size_t id_num = 0;
    for (int i = 0; i < 4; ++i) {
        thds[i]->join();
        id_num += 36768;

        if (i != 0) {
            s[0].insert(s[i].begin(), s[i].end());
        }
    }

    time_t end_time = time(NULL);
    CASE_EXPECT_EQ(id_num, s[0].size());
    CASE_EXPECT_GE(end_time - begin_time, (time_t)1);
}

#endif