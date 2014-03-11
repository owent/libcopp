#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine/coroutine_manager.h>


#define CHECK_STATUS(x, y) \
    (x) == (y) ? \
    printf("check %s == %s: PASS\n", #x, #y) : \
    printf("check %s == %s: FAILED!!!!!!!!!!\n", #x, #y)


int g_status = 0;
copp::default_coroutine_manager g_co_mgr;

class test_context_manager_runner1 : public copp::coroutine_manager_runner_base
{
public:
    typedef typename copp::default_coroutine_manager::value_type value_type;
public:
    int operator()() {
        puts("co 1 start.");
        CHECK_STATUS(++ g_status, 3);

        get_coroutine_context<value_type>()->yield();

        puts("co 1 resumed.");
        CHECK_STATUS(++ g_status, 6);

        return 0;
    }
};

class test_context_manager_runner2 : public copp::coroutine_manager_runner_base
{
public:
    typedef typename copp::default_coroutine_manager::value_type value_type;

    test_context_manager_runner2(uint64_t key): other_co_key(key){}

public:
    int operator()() {
        puts("co 2 start.");
        CHECK_STATUS(++ g_status, 2);

        g_co_mgr.start(other_co_key);

        CHECK_STATUS(++ g_status, 4);
        get_coroutine_context<value_type>()->yield();

        puts("co 2 resumed.");
        CHECK_STATUS(++g_status, 8);

        return 0;
    }

private:
    uint64_t other_co_key;
};

int main() {
    typedef typename copp::default_coroutine_manager::value_type co_type;
    puts("co create.");
    co_type* pco1 = g_co_mgr.create();
    co_type* pco2 = g_co_mgr.create();
    pco1->create_runner<test_context_manager_runner1>(64 * 1024);
    pco2->create_runner<test_context_manager_runner2>(64 * 1024, pco1->get_key());

    CHECK_STATUS(g_co_mgr.size(), 2);
    CHECK_STATUS(++ g_status, 1);

    g_co_mgr.start(pco2->get_key());
    CHECK_STATUS(++ g_status, 5);

    g_co_mgr.start(pco1->get_key());
    CHECK_STATUS(++ g_status, 7);

    g_co_mgr.start(pco2->get_key());
    CHECK_STATUS(++ g_status, 9);

    g_co_mgr.remove(pco1->get_key());
    CHECK_STATUS(g_co_mgr.size(), 1);
    return 0;
}
