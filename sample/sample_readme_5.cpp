/*
 * sample_readme_5.cpp
 *
 *  Created on: 2014-05-19
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>

#include <libcopp/utils/std/explicit_declare.h>

// include manager header file
#include <libcotask/task.h>

#if defined(LIBCOTASK_MACRO_ENABLED)

typedef cotask::task<> my_task_t;

int main() {
  int test_code = 128;

  // create a task using lambda expression
  my_task_t::ptr_t first_task = my_task_t::create([&]() {
    puts("|first task running and will be yield ...");
    cotask::this_task::get_task()->yield();
    puts("|first task resumed ...");
    printf("test code already reset => %d\n", ++test_code);
  });

  // add many then task using lambda expression
  first_task
      ->then([=]() {
        puts("|second task running...");
        printf("test code should be inited 128 => %d\n", test_code);
      })
      ->then([&]() {
        puts("|haha ... this is the third task.");
        printf("test code is the same => %d\n", ++test_code);
        return "return value will be ignored";
      })
      ->then(
          [&](EXPLICIT_UNUSED_ATTR void *priv_data) {
            puts("|it's boring");
            printf("test code is %d\n", ++test_code);
            assert(&test_code == priv_data);
            return 0;
          },
          &test_code);

  test_code = 0;
  // start a task
  first_task->start();
  first_task->resume();

  // these code below will failed.
  first_task->then([]() {
    puts("this will run immediately.");
    return 0;
  });

  my_task_t::ptr_t await_task = my_task_t::create([&]() {
    puts("await_task for first_task.");
    return 0;
  });
  await_task->await_task(first_task);

  printf("|task start twice will failed: %d\n", first_task->start());
  printf("|test_code end with %d\n", test_code);
  return 0;
}
#else
int main() {
  puts("this sample require cotask enabled");
  return 0;
}
#endif
