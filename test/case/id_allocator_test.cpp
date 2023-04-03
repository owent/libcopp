// Copyright 2023 owent

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <set>
#include <thread>

#include "frame/test_macros.h"

#include "libcopp/utils/uint64_id_allocator.h"

CASE_TEST(coroutine_task, id_allocator_st) {
  copp::util::uint64_id_allocator alloc;
  ((void)alloc);

  size_t id_num = 3 * (1 << 8) + 100;
  std::set<uint64_t> s;

  for (size_t i = 0; i < id_num; ++i) {
    uint64_t id = alloc.allocate();
    CASE_EXPECT_TRUE(s.find(id) == s.end());
    s.insert(id);
  }

  CASE_EXPECT_EQ(id_num, s.size());
}

CASE_TEST(coroutine_task, id_allocator_mt) {
  copp::util::uint64_id_allocator alloc;
  ((void)alloc);

  std::unique_ptr<std::thread> thds[40];
  std::set<uint64_t> s[40];
  for (int i = 0; i < 40; ++i) {
    std::set<uint64_t> *sp = &s[i];
    thds[i].reset(new std::thread([sp, &alloc]() {
      size_t id_num = 36768;

      for (size_t j = 0; j < id_num; ++j) {
        uint64_t id = alloc.allocate();
        CASE_EXPECT_TRUE(sp->find(id) == sp->end());
        sp->insert(id);
      }
    }));
  }

  size_t id_num = 0;
  for (int i = 0; i < 40; ++i) {
    thds[i]->join();
    id_num += 36768;

    if (i != 0) {
      s[0].insert(s[i].begin(), s[i].end());
    }
  }

  CASE_EXPECT_EQ(id_num, s[0].size());
}
