// Copyright 2025 owebt
// Licenses under the MIT License

#include <stdint.h>
#include <utility>

#include "frame/test_macros.h"

#include "libcopp/utils/config/libcopp_build_features.h"

#include "libcopp/utils/memory/rc_ptr.h"

#if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic push
#  endif
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif

namespace {
template <class X>
struct checked_deleter {
  void operator()(X *p) { delete p; }
};

template <class X>
struct checked_array_deleter {
  void operator()(X *p) { delete[] p; }
};

}  // namespace

namespace strong_rc {
namespace n_element_type {

void f(int &) {}

void test() {
  typedef copp::memory::strong_rc_ptr<int>::element_type T;
  T t;
  f(t);
}

}  // namespace n_element_type

namespace n_constructors {

class incomplete;

void default_constructor() {
  {
    copp::memory::strong_rc_ptr<int> pi;
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<void> pv;
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> px;
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 0);
  }
}

struct A {
  int dummy;
};

struct X {
  static int64_t instances;

  X() { ++instances; }

  ~X() { --instances; }

 private:
  X(X const &);
  X &operator=(X const &);
};

int64_t X::instances = 0;

// virtual inheritance stresses the implementation

struct Y : public A, public virtual X {
  static int64_t instances;

  Y() { ++instances; }

  ~Y() { --instances; }

 private:
  Y(Y const &);
  Y &operator=(Y const &);
};

int64_t Y::instances = 0;

template <class T>
void pc0_test(T *p) {
  CASE_EXPECT_TRUE(p == 0);
  copp::memory::strong_rc_ptr<T> pt(p);
  CASE_EXPECT_TRUE(pt ? false : true);
  CASE_EXPECT_TRUE(!pt);
  CASE_EXPECT_TRUE(pt.get() == 0);
  CASE_EXPECT_TRUE(pt.use_count() == 1);
  CASE_EXPECT_TRUE(pt.unique());
}

void pointer_constructor() {
  pc0_test(static_cast<int *>(0));

  pc0_test(static_cast<int const *>(0));
  pc0_test(static_cast<int volatile *>(0));
  pc0_test(static_cast<int const volatile *>(0));

  {
    copp::memory::strong_rc_ptr<int const> pi(static_cast<int *>(0));
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
  }

  {
    copp::memory::strong_rc_ptr<int volatile> pi(static_cast<int *>(0));
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
  }

  {
    copp::memory::strong_rc_ptr<void> pv(static_cast<int *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    copp::memory::strong_rc_ptr<void const> pv(static_cast<int *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  pc0_test(static_cast<X *>(0));
  pc0_test(static_cast<X const *>(0));
  pc0_test(static_cast<X volatile *>(0));
  pc0_test(static_cast<X const volatile *>(0));

  {
    copp::memory::strong_rc_ptr<X const> px(static_cast<X *>(0));
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
  }

  {
    copp::memory::strong_rc_ptr<X> px(static_cast<Y *>(0));
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
  }

  {
    copp::memory::strong_rc_ptr<X const> px(static_cast<Y *>(0));
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
  }

  {
    copp::memory::strong_rc_ptr<void> pv(static_cast<X *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    copp::memory::strong_rc_ptr<void const> pv(static_cast<X *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    int *p = new int(7);
    copp::memory::strong_rc_ptr<int> pi(p);
    CASE_EXPECT_TRUE(pi ? true : false);
    CASE_EXPECT_TRUE(!!pi);
    CASE_EXPECT_TRUE(pi.get() == p);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
    CASE_EXPECT_TRUE(*pi == 7);
  }

  {
    int *p = new int(7);
    copp::memory::strong_rc_ptr<int const> pi(p);
    CASE_EXPECT_TRUE(pi ? true : false);
    CASE_EXPECT_TRUE(!!pi);
    CASE_EXPECT_TRUE(pi.get() == p);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
    CASE_EXPECT_TRUE(*pi == 7);
  }

  {
    int *p = new int(7);
    copp::memory::strong_rc_ptr<void> pv(p);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == p);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    int *p = new int(7);
    copp::memory::strong_rc_ptr<void const> pv(p);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == p);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<X> px(p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<X const> px(p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<void> pv(p);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == p);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<void const> pv(p);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == p);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
  }

  CASE_EXPECT_TRUE(X::instances == 0);
  CASE_EXPECT_TRUE(Y::instances == 0);

  {
    Y *p = new Y;
    copp::memory::strong_rc_ptr<X> px(p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);
  }

  CASE_EXPECT_TRUE(X::instances == 0);
  CASE_EXPECT_TRUE(Y::instances == 0);

  {
    Y *p = new Y;
    copp::memory::strong_rc_ptr<X const> px(p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);
  }

  CASE_EXPECT_TRUE(X::instances == 0);
  CASE_EXPECT_TRUE(Y::instances == 0);
}

int m = 0;

void deleter(int *p) { CASE_EXPECT_TRUE(p == 0); }

void deleter2(int *p) {
  CASE_EXPECT_TRUE(p == &m);
  ++*p;
}

struct deleter3 {
  void operator()(incomplete *p) { CASE_EXPECT_TRUE(p == 0); }
};

// Borland C++ 5.5.1 fails on static_cast<incomplete*>(0)

incomplete *p0 = 0;

void deleter_constructor() {
  {
    copp::memory::strong_rc_ptr<int> pi(static_cast<int *>(0), deleter);
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
  }

  {
    copp::memory::strong_rc_ptr<void> pv(static_cast<int *>(0), &deleter);
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    copp::memory::strong_rc_ptr<void const> pv(static_cast<int *>(0), deleter);
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    copp::memory::strong_rc_ptr<incomplete> px(p0, deleter3());
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
  }

  {
    copp::memory::strong_rc_ptr<void> pv(p0, deleter3());
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  {
    copp::memory::strong_rc_ptr<void const> pv(p0, deleter3());
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  CASE_EXPECT_TRUE(m == 0);

  {
    copp::memory::strong_rc_ptr<int> pi(&m, deleter2);
    CASE_EXPECT_TRUE(pi ? true : false);
    CASE_EXPECT_TRUE(!!pi);
    CASE_EXPECT_TRUE(pi.get() == &m);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
  }

  CASE_EXPECT_TRUE(m == 1);

  {
    copp::memory::strong_rc_ptr<int const> pi(&m, &deleter2);
    CASE_EXPECT_TRUE(pi ? true : false);
    CASE_EXPECT_TRUE(!!pi);
    CASE_EXPECT_TRUE(pi.get() == &m);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
  }

  CASE_EXPECT_TRUE(m == 2);

  {
    copp::memory::strong_rc_ptr<void> pv(&m, deleter2);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == &m);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  CASE_EXPECT_TRUE(m == 3);

  {
    copp::memory::strong_rc_ptr<void const> pv(&m, &deleter2);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == &m);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
  }

  CASE_EXPECT_TRUE(m == 4);
}

void copy_constructor() {
  {
    copp::memory::strong_rc_ptr<int> pi;

    copp::memory::strong_rc_ptr<int> pi2(pi);
    CASE_EXPECT_TRUE(pi2 == pi);
    CASE_EXPECT_TRUE(pi2 ? false : true);
    CASE_EXPECT_TRUE(!pi2);
    CASE_EXPECT_TRUE(pi2.get() == 0);
    CASE_EXPECT_TRUE(pi2.use_count() == pi.use_count());

    copp::memory::strong_rc_ptr<void> pi3(pi);
    CASE_EXPECT_TRUE(pi3 == pi);
    CASE_EXPECT_TRUE(pi3 ? false : true);
    CASE_EXPECT_TRUE(!pi3);
    CASE_EXPECT_TRUE(pi3.get() == 0);
    CASE_EXPECT_TRUE(pi3.use_count() == pi.use_count());

    copp::memory::strong_rc_ptr<void> pi4(pi3);
    CASE_EXPECT_TRUE(pi4 == pi3);
    CASE_EXPECT_TRUE(pi4 ? false : true);
    CASE_EXPECT_TRUE(!pi4);
    CASE_EXPECT_TRUE(pi4.get() == 0);
    CASE_EXPECT_TRUE(pi4.use_count() == pi3.use_count());
  }

  {
    copp::memory::strong_rc_ptr<void> pv;

    copp::memory::strong_rc_ptr<void> pv2(pv);
    CASE_EXPECT_TRUE(pv2 == pv);
    CASE_EXPECT_TRUE(pv2 ? false : true);
    CASE_EXPECT_TRUE(!pv2);
    CASE_EXPECT_TRUE(pv2.get() == 0);
    CASE_EXPECT_TRUE(pv2.use_count() == pv.use_count());
  }

  {
    copp::memory::strong_rc_ptr<incomplete> px;

    copp::memory::strong_rc_ptr<incomplete> px2(px);
    CASE_EXPECT_TRUE(px2 == px);
    CASE_EXPECT_TRUE(px2 ? false : true);
    CASE_EXPECT_TRUE(!px2);
    CASE_EXPECT_TRUE(px2.get() == 0);
    CASE_EXPECT_TRUE(px2.use_count() == px.use_count());

    copp::memory::strong_rc_ptr<void> px3(px);
    CASE_EXPECT_TRUE(px3 == px);
    CASE_EXPECT_TRUE(px3 ? false : true);
    CASE_EXPECT_TRUE(!px3);
    CASE_EXPECT_TRUE(px3.get() == 0);
    CASE_EXPECT_TRUE(px3.use_count() == px.use_count());
  }

  {
    copp::memory::strong_rc_ptr<int> pi(static_cast<int *>(0));

    copp::memory::strong_rc_ptr<int> pi2(pi);
    CASE_EXPECT_TRUE(pi2 == pi);
    CASE_EXPECT_TRUE(pi2 ? false : true);
    CASE_EXPECT_TRUE(!pi2);
    CASE_EXPECT_TRUE(pi2.get() == 0);
    CASE_EXPECT_TRUE(pi2.use_count() == 2);
    CASE_EXPECT_TRUE(!pi2.unique());
    CASE_EXPECT_TRUE(pi2.use_count() == pi.use_count());
    CASE_EXPECT_TRUE(!(pi < pi2 || pi2 < pi));  // shared ownership test

    copp::memory::strong_rc_ptr<void> pi3(pi);
    CASE_EXPECT_TRUE(pi3 == pi);
    CASE_EXPECT_TRUE(pi3 ? false : true);
    CASE_EXPECT_TRUE(!pi3);
    CASE_EXPECT_TRUE(pi3.get() == 0);
    CASE_EXPECT_TRUE(pi3.use_count() == 3);
    CASE_EXPECT_TRUE(!pi3.unique());
    CASE_EXPECT_TRUE(pi3.use_count() == pi.use_count());
    CASE_EXPECT_TRUE(!(pi < pi3 || pi3 < pi));  // shared ownership test

    copp::memory::strong_rc_ptr<void> pi4(pi2);
    CASE_EXPECT_TRUE(pi4 == pi2);
    CASE_EXPECT_TRUE(pi4 ? false : true);
    CASE_EXPECT_TRUE(!pi4);
    CASE_EXPECT_TRUE(pi4.get() == 0);
    CASE_EXPECT_TRUE(pi4.use_count() == 4);
    CASE_EXPECT_TRUE(!pi4.unique());
    CASE_EXPECT_TRUE(pi4.use_count() == pi2.use_count());
    CASE_EXPECT_TRUE(!(pi2 < pi4 || pi4 < pi2));  // shared ownership test

    CASE_EXPECT_TRUE(pi3.use_count() == pi4.use_count());
    CASE_EXPECT_TRUE(!(pi3 < pi4 || pi4 < pi3));  // shared ownership test
  }

  {
    copp::memory::strong_rc_ptr<X> px(static_cast<X *>(0));

    copp::memory::strong_rc_ptr<X> px2(px);
    CASE_EXPECT_TRUE(px2 == px);
    CASE_EXPECT_TRUE(px2 ? false : true);
    CASE_EXPECT_TRUE(!px2);
    CASE_EXPECT_TRUE(px2.get() == 0);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(!px2.unique());
    CASE_EXPECT_TRUE(px2.use_count() == px.use_count());
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));  // shared ownership test

    copp::memory::strong_rc_ptr<void> px3(px);
    CASE_EXPECT_TRUE(px3 == px);
    CASE_EXPECT_TRUE(px3 ? false : true);
    CASE_EXPECT_TRUE(!px3);
    CASE_EXPECT_TRUE(px3.get() == 0);
    CASE_EXPECT_TRUE(px3.use_count() == 3);
    CASE_EXPECT_TRUE(!px3.unique());
    CASE_EXPECT_TRUE(px3.use_count() == px.use_count());
    CASE_EXPECT_TRUE(!(px < px3 || px3 < px));  // shared ownership test

    copp::memory::strong_rc_ptr<void> px4(px2);
    CASE_EXPECT_TRUE(px4 == px2);
    CASE_EXPECT_TRUE(px4 ? false : true);
    CASE_EXPECT_TRUE(!px4);
    CASE_EXPECT_TRUE(px4.get() == 0);
    CASE_EXPECT_TRUE(px4.use_count() == 4);
    CASE_EXPECT_TRUE(!px4.unique());
    CASE_EXPECT_TRUE(px4.use_count() == px2.use_count());
    CASE_EXPECT_TRUE(!(px2 < px4 || px4 < px2));  // shared ownership test

    CASE_EXPECT_TRUE(px3.use_count() == px4.use_count());
    CASE_EXPECT_TRUE(!(px3 < px4 || px4 < px3));  // shared ownership test
  }

  {
    int *p = new int(7);
    copp::memory::strong_rc_ptr<int> pi(p);

    copp::memory::strong_rc_ptr<int> pi2(pi);
    CASE_EXPECT_TRUE(pi2 == pi);
    CASE_EXPECT_TRUE(pi2 ? true : false);
    CASE_EXPECT_TRUE(!!pi2);
    CASE_EXPECT_TRUE(pi2.get() == p);
    CASE_EXPECT_TRUE(pi2.use_count() == 2);
    CASE_EXPECT_TRUE(!pi2.unique());
    CASE_EXPECT_TRUE(*pi2 == 7);
    CASE_EXPECT_TRUE(pi2.use_count() == pi.use_count());
    CASE_EXPECT_TRUE(!(pi < pi2 || pi2 < pi));  // shared ownership test
  }

  {
    int *p = new int(7);
    copp::memory::strong_rc_ptr<void> pv(p);
    CASE_EXPECT_TRUE(pv.get() == p);

    copp::memory::strong_rc_ptr<void> pv2(pv);
    CASE_EXPECT_TRUE(pv2 == pv);
    CASE_EXPECT_TRUE(pv2 ? true : false);
    CASE_EXPECT_TRUE(!!pv2);
    CASE_EXPECT_TRUE(pv2.get() == p);
    CASE_EXPECT_TRUE(pv2.use_count() == 2);
    CASE_EXPECT_TRUE(!pv2.unique());
    CASE_EXPECT_TRUE(pv2.use_count() == pv.use_count());
    CASE_EXPECT_TRUE(!(pv < pv2 || pv2 < pv));  // shared ownership test
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<X> px(p);
    CASE_EXPECT_TRUE(px.get() == p);

    copp::memory::strong_rc_ptr<X> px2(px);
    CASE_EXPECT_TRUE(px2 == px);
    CASE_EXPECT_TRUE(px2 ? true : false);
    CASE_EXPECT_TRUE(!!px2);
    CASE_EXPECT_TRUE(px2.get() == p);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(!px2.unique());

    CASE_EXPECT_TRUE(X::instances == 1);

    CASE_EXPECT_TRUE(px2.use_count() == px.use_count());
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));  // shared ownership test

    copp::memory::strong_rc_ptr<void> px3(px);
    CASE_EXPECT_TRUE(px3 == px);
    CASE_EXPECT_TRUE(px3 ? true : false);
    CASE_EXPECT_TRUE(!!px3);
    CASE_EXPECT_TRUE(px3.get() == p);
    CASE_EXPECT_TRUE(px3.use_count() == 3);
    CASE_EXPECT_TRUE(!px3.unique());
    CASE_EXPECT_TRUE(px3.use_count() == px.use_count());
    CASE_EXPECT_TRUE(!(px < px3 || px3 < px));  // shared ownership test

    copp::memory::strong_rc_ptr<void> px4(px2);
    CASE_EXPECT_TRUE(px4 == px2);
    CASE_EXPECT_TRUE(px4 ? true : false);
    CASE_EXPECT_TRUE(!!px4);
    CASE_EXPECT_TRUE(px4.get() == p);
    CASE_EXPECT_TRUE(px4.use_count() == 4);
    CASE_EXPECT_TRUE(!px4.unique());
    CASE_EXPECT_TRUE(px4.use_count() == px2.use_count());
    CASE_EXPECT_TRUE(!(px2 < px4 || px4 < px2));  // shared ownership test

    CASE_EXPECT_TRUE(px3.use_count() == px4.use_count());
    CASE_EXPECT_TRUE(!(px3 < px4 || px4 < px3));  // shared ownership test
  }

  CASE_EXPECT_TRUE(X::instances == 0);
  CASE_EXPECT_TRUE(Y::instances == 0);

  {
    Y *p = new Y;
    copp::memory::strong_rc_ptr<Y> py(p);
    CASE_EXPECT_TRUE(py.get() == p);

    copp::memory::strong_rc_ptr<X> px(py);
    CASE_EXPECT_TRUE(px == py);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(!px.unique());
    CASE_EXPECT_TRUE(px.use_count() == py.use_count());
    CASE_EXPECT_TRUE(!(px < py || py < px));  // shared ownership test

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);

    copp::memory::strong_rc_ptr<void const> pv(px);
    CASE_EXPECT_TRUE(pv == px);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == px.get());
    CASE_EXPECT_TRUE(pv.use_count() == 3);
    CASE_EXPECT_TRUE(!pv.unique());
    CASE_EXPECT_TRUE(pv.use_count() == px.use_count());
    CASE_EXPECT_TRUE(!(px < pv || pv < px));  // shared ownership test

    copp::memory::strong_rc_ptr<void const> pv2(py);
    CASE_EXPECT_TRUE(pv2 == py);
    CASE_EXPECT_TRUE(pv2 ? true : false);
    CASE_EXPECT_TRUE(!!pv2);
    CASE_EXPECT_TRUE(pv2.get() == py.get());
    CASE_EXPECT_TRUE(pv2.use_count() == 4);
    CASE_EXPECT_TRUE(!pv2.unique());
    CASE_EXPECT_TRUE(pv2.use_count() == py.use_count());
    CASE_EXPECT_TRUE(!(py < pv2 || pv2 < py));  // shared ownership test
  }

  CASE_EXPECT_TRUE(X::instances == 0);
  CASE_EXPECT_TRUE(Y::instances == 0);
}

void weak_ptr_constructor() {
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
  {
    copp::memory::weak_rc_ptr<Y> wp;
    CASE_EXPECT_TRUE(wp.use_count() == 0);

    try {
      copp::memory::strong_rc_ptr<Y> p2(wp);
      CASE_EXPECT_ERROR("shared_ptr<Y> p2(wp) failed to throw");
    } catch (std::bad_weak_ptr const &) {
    }

    try {
      copp::memory::strong_rc_ptr<X> p3(wp);
      CASE_EXPECT_ERROR("shared_ptr<X> p3(wp) failed to throw");
    } catch (std::bad_weak_ptr const &) {
    }
  }
#endif

  {
    copp::memory::strong_rc_ptr<Y> p;
    copp::memory::weak_rc_ptr<Y> wp(p);

    // 0 allowed but not required
    if (wp.use_count() != 0) {
      copp::memory::strong_rc_ptr<Y> p2(wp);
      CASE_EXPECT_TRUE(p2.use_count() == wp.use_count());
      CASE_EXPECT_TRUE(p2.get() == 0);

      copp::memory::strong_rc_ptr<X> p3(wp);
      CASE_EXPECT_TRUE(p3.use_count() == wp.use_count());
      CASE_EXPECT_TRUE(p3.get() == 0);
    }
  }

  {
    copp::memory::strong_rc_ptr<Y> p(new Y);
    copp::memory::weak_rc_ptr<Y> wp(p);

    {
      copp::memory::strong_rc_ptr<Y> p2(wp);
      CASE_EXPECT_TRUE(p2 ? true : false);
      CASE_EXPECT_TRUE(!!p2);
      CASE_EXPECT_TRUE(p2.get() == p.get());
      CASE_EXPECT_TRUE(p2.use_count() == 2);
      CASE_EXPECT_TRUE(!p2.unique());
      CASE_EXPECT_TRUE(p2.use_count() == wp.use_count());

      CASE_EXPECT_TRUE(p.use_count() == p2.use_count());
      CASE_EXPECT_TRUE(!(p < p2 || p2 < p));  // shared ownership test

      copp::memory::strong_rc_ptr<X> p3(wp);
      CASE_EXPECT_TRUE(p3 ? true : false);
      CASE_EXPECT_TRUE(!!p3);
      CASE_EXPECT_TRUE(p3.get() == p.get());
      CASE_EXPECT_TRUE(p3.use_count() == 3);
      CASE_EXPECT_TRUE(!p3.unique());
      CASE_EXPECT_TRUE(p3.use_count() == wp.use_count());

      CASE_EXPECT_TRUE(p.use_count() == p3.use_count());
    }

    p.reset();
    CASE_EXPECT_TRUE(wp.use_count() == 0);

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<Y> p2(wp);
      CASE_EXPECT_ERROR("shared_ptr<Y> p2(wp) failed to throw");
    } catch (std::bad_weak_ptr const &) {
    }

    try {
      copp::memory::strong_rc_ptr<X> p3(wp);
      CASE_EXPECT_ERROR("shared_ptr<X> p3(wp) failed to throw");
    } catch (std::bad_weak_ptr const &) {
    }
#endif
  }
}

void test() {
  default_constructor();
  pointer_constructor();
  deleter_constructor();
  copy_constructor();
  weak_ptr_constructor();
}

}  // namespace n_constructors

namespace n_assignment {

class incomplete;

struct A {
  int dummy;
};

struct X {
  static int64_t instances;

  X() { ++instances; }

  ~X() { --instances; }

 private:
  X(X const &);
  X &operator=(X const &);
};

int64_t X::instances = 0;

struct Y : public A, public virtual X {
  static int64_t instances;

  Y() { ++instances; }

  ~Y() { --instances; }

 private:
  Y(Y const &);
  Y &operator=(Y const &);
};

int64_t Y::instances = 0;

void copy_assignment() {
  {
    copp::memory::strong_rc_ptr<incomplete> p1;

    p1 = p1;

    CASE_EXPECT_TRUE(p1 == p1);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<incomplete> p2;

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<incomplete> p3(p1);

    p1 = p3;

    CASE_EXPECT_TRUE(p1 == p3);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);
  }

  {
    copp::memory::strong_rc_ptr<void> p1;

    p1 = p1;

    CASE_EXPECT_TRUE(p1 == p1);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<void> p2;

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<void> p3(p1);

    p1 = p3;

    CASE_EXPECT_TRUE(p1 == p3);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<void> p4(new int);
    CASE_EXPECT_TRUE(p4.use_count() == 1);

    p1 = p4;

    CASE_EXPECT_TRUE(p1 == p4);
    CASE_EXPECT_TRUE(!(p1 < p4 || p4 < p1));
    CASE_EXPECT_TRUE(p1.use_count() == 2);
    CASE_EXPECT_TRUE(p4.use_count() == 2);

    p1 = p3;

    CASE_EXPECT_TRUE(p1 == p3);
    CASE_EXPECT_TRUE(p4.use_count() == 1);
  }

  {
    copp::memory::strong_rc_ptr<X> p1;

    p1 = p1;

    CASE_EXPECT_TRUE(p1 == p1);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<X> p2;

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<X> p3(p1);

    p1 = p3;

    CASE_EXPECT_TRUE(p1 == p3);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    CASE_EXPECT_TRUE(X::instances == 0);

    copp::memory::strong_rc_ptr<X> p4(new X);

    CASE_EXPECT_TRUE(X::instances == 1);

    p1 = p4;

    CASE_EXPECT_TRUE(X::instances == 1);

    CASE_EXPECT_TRUE(p1 == p4);
    CASE_EXPECT_TRUE(!(p1 < p4 || p4 < p1));

    CASE_EXPECT_TRUE(p1.use_count() == 2);

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(X::instances == 1);

    p4 = p3;

    CASE_EXPECT_TRUE(p4 == p3);
    CASE_EXPECT_TRUE(X::instances == 0);
  }
}

void conversion_assignment() {
  {
    copp::memory::strong_rc_ptr<void> p1;

    copp::memory::strong_rc_ptr<incomplete> p2;

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    copp::memory::strong_rc_ptr<int> p4(new int);
    CASE_EXPECT_TRUE(p4.use_count() == 1);

    copp::memory::strong_rc_ptr<void> p5(p4);
    CASE_EXPECT_TRUE(p4.use_count() == 2);

    p1 = p4;

    CASE_EXPECT_TRUE(p1 == p4);
    CASE_EXPECT_TRUE(!(p1 < p5 || p5 < p1));
    CASE_EXPECT_TRUE(p1.use_count() == 3);
    CASE_EXPECT_TRUE(p4.use_count() == 3);

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(p4.use_count() == 2);
  }

  {
    copp::memory::strong_rc_ptr<X> p1;

    copp::memory::strong_rc_ptr<Y> p2;

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(p1 ? false : true);
    CASE_EXPECT_TRUE(!p1);
    CASE_EXPECT_TRUE(p1.get() == 0);

    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(Y::instances == 0);

    copp::memory::strong_rc_ptr<Y> p4(new Y);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);
    CASE_EXPECT_TRUE(p4.use_count() == 1);

    copp::memory::strong_rc_ptr<X> p5(p4);
    CASE_EXPECT_TRUE(p4.use_count() == 2);

    p1 = p4;

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);

    CASE_EXPECT_TRUE(p1 == p4);
    CASE_EXPECT_TRUE(!(p1 < p5 || p5 < p1));

    CASE_EXPECT_TRUE(p1.use_count() == 3);
    CASE_EXPECT_TRUE(p4.use_count() == 3);

    p1 = p2;

    CASE_EXPECT_TRUE(p1 == p2);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);
    CASE_EXPECT_TRUE(p4.use_count() == 2);

    p4 = p2;
    p5 = p2;

    CASE_EXPECT_TRUE(p4 == p2);
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(Y::instances == 0);
  }
}

void test() {
  copy_assignment();
  conversion_assignment();
}

}  // namespace n_assignment

namespace n_reset {

class incomplete;

incomplete *p0 = 0;

void deleter(incomplete *) {}

struct X {
  static int64_t instances;

  X() { ++instances; }

  ~X() { --instances; }

 private:
  X(X const &);
  X &operator=(X const &);
};

int64_t X::instances = 0;

void plain_reset() {
  {
    copp::memory::strong_rc_ptr<int> pi;
    pi.reset();
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> pi(static_cast<int *>(0));
    pi.reset();
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> pi(new int);
    pi.reset();
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> px;
    px.reset();
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> px(p0, deleter);
    px.reset();
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> px;
    px.reset();
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 0);
  }

  {
    CASE_EXPECT_TRUE(X::instances == 0);
    copp::memory::strong_rc_ptr<X> px(new X);
    CASE_EXPECT_TRUE(X::instances == 1);
    px.reset();
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 0);
    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<void> pv;
    pv.reset();
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 0);
  }

  {
    CASE_EXPECT_TRUE(X::instances == 0);
    copp::memory::strong_rc_ptr<void> pv(new X);
    CASE_EXPECT_TRUE(X::instances == 1);
    pv.reset();
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 0);
    CASE_EXPECT_TRUE(X::instances == 0);
  }
}

struct A {
  int dummy;
};

struct Y : public A, public virtual X {
  static int64_t instances;

  Y() { ++instances; }

  ~Y() { --instances; }

 private:
  Y(Y const &);
  Y &operator=(Y const &);
};

int64_t Y::instances = 0;

void pointer_reset() {
  {
    copp::memory::strong_rc_ptr<int> pi;
    CASE_EXPECT_TRUE(pi.use_count() == 0);

    pi.reset(static_cast<int *>(nullptr));
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());

    int *p = new int;
    pi.reset(p);
    CASE_EXPECT_TRUE(pi ? true : false);
    CASE_EXPECT_TRUE(!!pi);
    CASE_EXPECT_TRUE(pi.get() == p);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());

    pi.reset(static_cast<int *>(0));
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());
  }

  {
    copp::memory::strong_rc_ptr<X> px;

    px.reset(static_cast<X *>(0));
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 0);

    X *p = new X;
    px.reset(p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 1);

    px.reset(static_cast<X *>(0));
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(Y::instances == 0);

    Y *q = new Y;
    px.reset(q);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == q);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);

    px.reset(static_cast<Y *>(0));
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(Y::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<void> pv;

    pv.reset(static_cast<X *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 0);

    X *p = new X;
    pv.reset(p);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == p);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 1);

    pv.reset(static_cast<X *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(Y::instances == 0);

    Y *q = new Y;
    pv.reset(q);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == q);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(Y::instances == 1);

    pv.reset(static_cast<Y *>(0));
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(Y::instances == 0);
  }
}

void *deleted = 0;

void deleter2(void *p) { deleted = p; }

void deleter_reset() {
  {
    copp::memory::strong_rc_ptr<int> pi;

    pi.reset(static_cast<int *>(0), deleter2);
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());

    deleted = &pi;

    int m = 0;
    pi.reset(&m, deleter2);
    CASE_EXPECT_TRUE(deleted == 0);
    CASE_EXPECT_TRUE(pi ? true : false);
    CASE_EXPECT_TRUE(!!pi);
    CASE_EXPECT_TRUE(pi.get() == &m);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());

    pi.reset(static_cast<int *>(0), deleter2);
    CASE_EXPECT_TRUE(deleted == &m);
    CASE_EXPECT_TRUE(pi ? false : true);
    CASE_EXPECT_TRUE(!pi);
    CASE_EXPECT_TRUE(pi.get() == 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi.unique());

    pi.reset();
    CASE_EXPECT_TRUE(deleted == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> px;

    px.reset(static_cast<X *>(0), deleter2);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    deleted = &px;

    X x;
    px.reset(&x, deleter2);
    CASE_EXPECT_TRUE(deleted == 0);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == &x);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    px.reset(static_cast<X *>(0), deleter2);
    CASE_EXPECT_TRUE(deleted == &x);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    Y y;
    px.reset(&y, deleter2);
    CASE_EXPECT_TRUE(deleted == 0);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(px.get() == &y);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    px.reset(static_cast<Y *>(0), deleter2);
    CASE_EXPECT_TRUE(deleted == &y);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    px.reset();
    CASE_EXPECT_TRUE(deleted == 0);
  }

  {
    copp::memory::strong_rc_ptr<void> pv;

    pv.reset(static_cast<X *>(0), deleter2);
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());

    deleted = &pv;

    X x;
    pv.reset(&x, deleter2);
    CASE_EXPECT_TRUE(deleted == 0);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == &x);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());

    pv.reset(static_cast<X *>(0), deleter2);
    CASE_EXPECT_TRUE(deleted == &x);
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());

    Y y;
    pv.reset(&y, deleter2);
    CASE_EXPECT_TRUE(deleted == 0);
    CASE_EXPECT_TRUE(pv ? true : false);
    CASE_EXPECT_TRUE(!!pv);
    CASE_EXPECT_TRUE(pv.get() == &y);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());

    pv.reset(static_cast<Y *>(0), deleter2);
    CASE_EXPECT_TRUE(deleted == &y);
    CASE_EXPECT_TRUE(pv ? false : true);
    CASE_EXPECT_TRUE(!pv);
    CASE_EXPECT_TRUE(pv.get() == 0);
    CASE_EXPECT_TRUE(pv.use_count() == 1);
    CASE_EXPECT_TRUE(pv.unique());

    pv.reset();
    CASE_EXPECT_TRUE(deleted == 0);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> px;

    px.reset(p0, deleter2);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    deleted = &px;
    px.reset(p0, deleter2);
    CASE_EXPECT_TRUE(deleted == 0);
  }
}

void test() {
  plain_reset();
  pointer_reset();
  deleter_reset();
}

}  // namespace n_reset

namespace n_access {

struct X {};

void test() {
  {
    copp::memory::strong_rc_ptr<X> px;
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
  }

  {
    copp::memory::strong_rc_ptr<X> px(static_cast<X *>(0));
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
  }

  {
    copp::memory::strong_rc_ptr<X> px(static_cast<X *>(0), checked_deleter<X>());
    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px ? false : true);
    CASE_EXPECT_TRUE(!px);
  }

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<X> px(p);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(&*px == px.get());
    CASE_EXPECT_TRUE(px.operator->() == px.get());
  }

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<X> px(p, checked_deleter<X>());
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px ? true : false);
    CASE_EXPECT_TRUE(!!px);
    CASE_EXPECT_TRUE(&*px == px.get());
    CASE_EXPECT_TRUE(px.operator->() == px.get());
  }
}

}  // namespace n_access

namespace n_use_count {

struct X {};

void test() {
  {
    copp::memory::strong_rc_ptr<X> px(static_cast<X *>(0));
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    copp::memory::strong_rc_ptr<X> px2(px);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(!px2.unique());
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(!px.unique());
  }

  {
    copp::memory::strong_rc_ptr<X> px(new X);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    copp::memory::strong_rc_ptr<X> px2(px);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(!px2.unique());
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(!px.unique());
  }

  {
    copp::memory::strong_rc_ptr<X> px(new X, checked_deleter<X>());
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px.unique());

    copp::memory::strong_rc_ptr<X> px2(px);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(!px2.unique());
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(!px.unique());
  }
}

}  // namespace n_use_count

namespace n_swap {

struct X {};

void test() {
  {
    copp::memory::strong_rc_ptr<X> px;
    copp::memory::strong_rc_ptr<X> px2;

    px.swap(px2);

    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px2.get() == 0);

    using std::swap;
    swap(px, px2);

    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px2.get() == 0);
  }

  {
    X *p = new X;
    copp::memory::strong_rc_ptr<X> px;
    copp::memory::strong_rc_ptr<X> px2(p);
    copp::memory::strong_rc_ptr<X> px3(px2);

    px.swap(px2);

    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(px2.get() == 0);
    CASE_EXPECT_TRUE(px3.get() == p);
    CASE_EXPECT_TRUE(px3.use_count() == 2);

    using std::swap;
    swap(px, px2);

    CASE_EXPECT_TRUE(px.get() == 0);
    CASE_EXPECT_TRUE(px2.get() == p);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(px3.get() == p);
    CASE_EXPECT_TRUE(px3.use_count() == 2);
  }

  {
    X *p1 = new X;
    X *p2 = new X;
    copp::memory::strong_rc_ptr<X> px(p1);
    copp::memory::strong_rc_ptr<X> px2(p2);
    copp::memory::strong_rc_ptr<X> px3(px2);

    px.swap(px2);

    CASE_EXPECT_TRUE(px.get() == p2);
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(px2.get() == p1);
    CASE_EXPECT_TRUE(px2.use_count() == 1);
    CASE_EXPECT_TRUE(px3.get() == p2);
    CASE_EXPECT_TRUE(px3.use_count() == 2);

    using std::swap;
    swap(px, px2);

    CASE_EXPECT_TRUE(px.get() == p1);
    CASE_EXPECT_TRUE(px.use_count() == 1);
    CASE_EXPECT_TRUE(px2.get() == p2);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
    CASE_EXPECT_TRUE(px3.get() == p2);
    CASE_EXPECT_TRUE(px3.use_count() == 2);
  }
}

}  // namespace n_swap

namespace n_comparison {

struct X {
  int dummy;
};

struct Y {
  int dummy2;
};

struct Z : public X, public virtual Y {};

void test() {
  {
    copp::memory::strong_rc_ptr<X> px;
    CASE_EXPECT_TRUE(px == px);
    CASE_EXPECT_TRUE(!(px != px));
    CASE_EXPECT_TRUE(!(px < px));

    copp::memory::strong_rc_ptr<X> px2;

    CASE_EXPECT_TRUE(px.get() == px2.get());
    CASE_EXPECT_TRUE(px == px2);
    CASE_EXPECT_TRUE(!(px != px2));
    CASE_EXPECT_TRUE(!(px < px2 && px2 < px));
  }

  {
    copp::memory::strong_rc_ptr<X> px;
    copp::memory::strong_rc_ptr<X> px2(px);

    CASE_EXPECT_TRUE(px2 == px2);
    CASE_EXPECT_TRUE(!(px2 != px2));
    CASE_EXPECT_TRUE(!(px2 < px2));

    CASE_EXPECT_TRUE(px.get() == px2.get());
    CASE_EXPECT_TRUE(px == px2);
    CASE_EXPECT_TRUE(!(px != px2));
    CASE_EXPECT_TRUE(!(px < px2 && px2 < px));
  }

  {
    copp::memory::strong_rc_ptr<X> px;
    copp::memory::strong_rc_ptr<X> px2(new X);

    CASE_EXPECT_TRUE(px2 == px2);
    CASE_EXPECT_TRUE(!(px2 != px2));
    CASE_EXPECT_TRUE(!(px2 < px2));

    CASE_EXPECT_TRUE(px.get() != px2.get());
    CASE_EXPECT_TRUE(px != px2);
    CASE_EXPECT_TRUE(!(px == px2));
    CASE_EXPECT_TRUE(px < px2 || px2 < px);
    CASE_EXPECT_TRUE(!(px < px2 && px2 < px));
  }

  {
    copp::memory::strong_rc_ptr<X> px(new X);
    copp::memory::strong_rc_ptr<X> px2(new X);

    CASE_EXPECT_TRUE(px.get() != px2.get());
    CASE_EXPECT_TRUE(px != px2);
    CASE_EXPECT_TRUE(!(px == px2));
    CASE_EXPECT_TRUE(px < px2 || px2 < px);
    CASE_EXPECT_TRUE(!(px < px2 && px2 < px));
  }

  {
    copp::memory::strong_rc_ptr<X> px(new X);
    copp::memory::strong_rc_ptr<X> px2(px);

    CASE_EXPECT_TRUE(px2 == px2);
    CASE_EXPECT_TRUE(!(px2 != px2));
    CASE_EXPECT_TRUE(!(px2 < px2));

    CASE_EXPECT_TRUE(px.get() == px2.get());
    CASE_EXPECT_TRUE(px == px2);
    CASE_EXPECT_TRUE(!(px != px2));
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));
  }

  {
    copp::memory::strong_rc_ptr<X> px(new X);
    copp::memory::strong_rc_ptr<Y> py(new Y);
    copp::memory::strong_rc_ptr<Z> pz(new Z);

    CASE_EXPECT_TRUE(px.get() != pz.get());
    CASE_EXPECT_TRUE(px != pz);
    CASE_EXPECT_TRUE(!(px == pz));

    CASE_EXPECT_TRUE(py.get() != pz.get());
    CASE_EXPECT_TRUE(py != pz);
    CASE_EXPECT_TRUE(!(py == pz));

    CASE_EXPECT_TRUE(px < py || py < px);
    CASE_EXPECT_TRUE(px < pz || pz < px);
    CASE_EXPECT_TRUE(py < pz || pz < py);

    CASE_EXPECT_TRUE(!(px < py && py < px));
    CASE_EXPECT_TRUE(!(px < pz && pz < px));
    CASE_EXPECT_TRUE(!(py < pz && pz < py));

    copp::memory::strong_rc_ptr<void> pvx(px);

    CASE_EXPECT_TRUE(pvx == pvx);
    CASE_EXPECT_TRUE(!(pvx != pvx));
    CASE_EXPECT_TRUE(!(pvx < pvx));

    copp::memory::strong_rc_ptr<void> pvy(py);
    copp::memory::strong_rc_ptr<void> pvz(pz);

    CASE_EXPECT_TRUE(pvx < pvy || pvy < pvx);
    CASE_EXPECT_TRUE(pvx < pvz || pvz < pvx);
    CASE_EXPECT_TRUE(pvy < pvz || pvz < pvy);

    CASE_EXPECT_TRUE(!(pvx < pvy && pvy < pvx));
    CASE_EXPECT_TRUE(!(pvx < pvz && pvz < pvx));
    CASE_EXPECT_TRUE(!(pvy < pvz && pvz < pvy));
  }

  {
    copp::memory::strong_rc_ptr<Z> pz(new Z);
    copp::memory::strong_rc_ptr<X> px(pz);

    CASE_EXPECT_TRUE(px == px);
    CASE_EXPECT_TRUE(!(px != px));
    CASE_EXPECT_TRUE(!(px < px));

    copp::memory::strong_rc_ptr<Y> py(pz);

    CASE_EXPECT_TRUE(px.get() == pz.get());
    CASE_EXPECT_TRUE(px == pz);
    CASE_EXPECT_TRUE(!(px != pz));

    CASE_EXPECT_TRUE(py.get() == pz.get());
    CASE_EXPECT_TRUE(py == pz);
    CASE_EXPECT_TRUE(!(py != pz));

    CASE_EXPECT_TRUE(px < py || py < px);
    CASE_EXPECT_TRUE(!(px < pz || pz < px));
    CASE_EXPECT_TRUE(!(py < pz || pz < py));

    copp::memory::strong_rc_ptr<void> pvx(px);
    copp::memory::strong_rc_ptr<void> pvy(py);
    copp::memory::strong_rc_ptr<void> pvz(pz);

    // pvx and pvy aren't equal...
    CASE_EXPECT_TRUE(pvx.get() != pvy.get());
    CASE_EXPECT_TRUE(pvx != pvy);
    CASE_EXPECT_TRUE(!(pvx == pvy));

    // ... but they share ownership ...
    CASE_EXPECT_TRUE(pvx < pvy || pvy < pvx);

    // ... with pvz
    // CASE_EXPECT_TRUE(!(pvx < pvz || pvz < pvx));
    CASE_EXPECT_TRUE(pvy < pvz || pvz < pvy);
  }
}

}  // namespace n_comparison

namespace n_static_cast {

struct X {};

struct Y : public X {};

void test() {
  {
    copp::memory::strong_rc_ptr<void> pv;

    copp::memory::strong_rc_ptr<int> pi = copp::memory::static_pointer_cast<int>(pv);
    CASE_EXPECT_TRUE(pi.get() == 0);

    copp::memory::strong_rc_ptr<X> px = copp::memory::static_pointer_cast<X>(pv);
    CASE_EXPECT_TRUE(px.get() == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> pi(new int);
    copp::memory::strong_rc_ptr<void> pv(pi);

    copp::memory::strong_rc_ptr<int> pi2 = copp::memory::static_pointer_cast<int>(pv);
    CASE_EXPECT_TRUE(pi.get() == pi2.get());
    CASE_EXPECT_TRUE(!(pi < pi2 || pi2 < pi));
    CASE_EXPECT_TRUE(pi.use_count() == 3);
    CASE_EXPECT_TRUE(pv.use_count() == 3);
    CASE_EXPECT_TRUE(pi2.use_count() == 3);
  }

  {
    copp::memory::strong_rc_ptr<X> px(new X);
    copp::memory::strong_rc_ptr<void> pv(px);

    copp::memory::strong_rc_ptr<X> px2 = copp::memory::static_pointer_cast<X>(pv);
    CASE_EXPECT_TRUE(px.get() == px2.get());
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));
    CASE_EXPECT_TRUE(px.use_count() == 3);
    CASE_EXPECT_TRUE(pv.use_count() == 3);
    CASE_EXPECT_TRUE(px2.use_count() == 3);
  }

  {
    copp::memory::strong_rc_ptr<X> px(new Y);

    copp::memory::strong_rc_ptr<Y> py = copp::memory::static_pointer_cast<Y>(px);
    CASE_EXPECT_TRUE(px.get() == py.get());
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(py.use_count() == 2);

    copp::memory::strong_rc_ptr<X> px2(py);
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));
  }
}

}  // namespace n_static_cast

namespace n_const_cast {

struct X;

void test() {
  {
    copp::memory::strong_rc_ptr<void const volatile> px;

    copp::memory::strong_rc_ptr<void> px2 = copp::memory::const_pointer_cast<void>(px);
    CASE_EXPECT_TRUE(px2.get() == 0);
  }

  {
    copp::memory::strong_rc_ptr<int const volatile> px;

    copp::memory::strong_rc_ptr<int> px2 = copp::memory::const_pointer_cast<int>(px);
    CASE_EXPECT_TRUE(px2.get() == 0);
  }

  {
    copp::memory::strong_rc_ptr<X const volatile> px;

    copp::memory::strong_rc_ptr<X> px2 = copp::memory::const_pointer_cast<X>(px);
    CASE_EXPECT_TRUE(px2.get() == 0);
  }

  {
    copp::memory::strong_rc_ptr<void const volatile> px(new int);

    copp::memory::strong_rc_ptr<void> px2 = copp::memory::const_pointer_cast<void>(px);
    CASE_EXPECT_TRUE(px.get() == px2.get());
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
  }

  {
    copp::memory::strong_rc_ptr<int const volatile> px(new int);

    copp::memory::strong_rc_ptr<int> px2 = copp::memory::const_pointer_cast<int>(px);
    CASE_EXPECT_TRUE(px.get() == px2.get());
    CASE_EXPECT_TRUE(!(px < px2 || px2 < px));
    CASE_EXPECT_TRUE(px.use_count() == 2);
    CASE_EXPECT_TRUE(px2.use_count() == 2);
  }
}

}  // namespace n_const_cast

#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI

namespace n_dynamic_cast {

struct V {
  virtual ~V() {}
};

struct W : public V {};

void test() {
  {
    copp::memory::strong_rc_ptr<V> pv;
    copp::memory::strong_rc_ptr<W> pw = copp::memory::dynamic_pointer_cast<W>(pv);
    CASE_EXPECT_TRUE(pw.get() == 0);
  }

  {
    copp::memory::strong_rc_ptr<V> pv(static_cast<V *>(0));

    copp::memory::strong_rc_ptr<W> pw = copp::memory::dynamic_pointer_cast<W>(pv);
    CASE_EXPECT_TRUE(pw.get() == 0);

    copp::memory::strong_rc_ptr<V> pv2(pw);
    CASE_EXPECT_TRUE(!(pv < pv2 || pv2 < pv));
  }

  {
    copp::memory::strong_rc_ptr<V> pv(static_cast<W *>(0));

    copp::memory::strong_rc_ptr<W> pw = copp::memory::dynamic_pointer_cast<W>(pv);
    CASE_EXPECT_TRUE(pw.get() == 0);

    copp::memory::strong_rc_ptr<V> pv2(pw);
    CASE_EXPECT_TRUE(!(pv < pv2 || pv2 < pv));
  }

  {
    copp::memory::strong_rc_ptr<V> pv(new V);

    copp::memory::strong_rc_ptr<W> pw = copp::memory::dynamic_pointer_cast<W>(pv);
    CASE_EXPECT_TRUE(pw.get() == 0);

    copp::memory::strong_rc_ptr<V> pv2(pw);
    CASE_EXPECT_TRUE(pv < pv2 || pv2 < pv);
  }

  {
    copp::memory::strong_rc_ptr<V> pv(new W);

    copp::memory::strong_rc_ptr<W> pw = copp::memory::dynamic_pointer_cast<W>(pv);
    CASE_EXPECT_TRUE(pw.get() == pv.get());
    CASE_EXPECT_TRUE(pv.use_count() == 2);
    CASE_EXPECT_TRUE(pw.use_count() == 2);

    copp::memory::strong_rc_ptr<V> pv2(pw);
    CASE_EXPECT_TRUE(!(pv < pv2 || pv2 < pv));
  }
}

}  // namespace n_dynamic_cast

#endif

namespace n_map {

struct X {};

void test() {
  std::vector<copp::memory::strong_rc_ptr<int>> vi;

  {
    copp::memory::strong_rc_ptr<int> pi1(new int);
    copp::memory::strong_rc_ptr<int> pi2(new int);
    copp::memory::strong_rc_ptr<int> pi3(new int);

    vi.push_back(pi1);
    vi.push_back(pi1);
    vi.push_back(pi1);
    vi.push_back(pi2);
    vi.push_back(pi1);
    vi.push_back(pi2);
    vi.push_back(pi1);
    vi.push_back(pi3);
    vi.push_back(pi3);
    vi.push_back(pi2);
    vi.push_back(pi1);
  }

  std::vector<copp::memory::strong_rc_ptr<X>> vx;

  {
    copp::memory::strong_rc_ptr<X> px1(new X);
    copp::memory::strong_rc_ptr<X> px2(new X);
    copp::memory::strong_rc_ptr<X> px3(new X);

    vx.push_back(px2);
    vx.push_back(px2);
    vx.push_back(px1);
    vx.push_back(px2);
    vx.push_back(px1);
    vx.push_back(px1);
    vx.push_back(px1);
    vx.push_back(px2);
    vx.push_back(px1);
    vx.push_back(px3);
    vx.push_back(px2);
  }

  std::map<copp::memory::strong_rc_ptr<void>, int64_t> m;

  {
    for (std::vector<copp::memory::strong_rc_ptr<int>>::iterator i = vi.begin(); i != vi.end(); ++i) {
      ++m[*i];
    }
  }

  {
    for (std::vector<copp::memory::strong_rc_ptr<X>>::iterator i = vx.begin(); i != vx.end(); ++i) {
      ++m[*i];
    }
  }

  {
    for (std::map<copp::memory::strong_rc_ptr<void>, int64_t>::iterator i = m.begin(); i != m.end(); ++i) {
      CASE_EXPECT_TRUE(i->first.use_count() == static_cast<std::size_t>(i->second + 1));
    }
  }
}

}  // namespace n_map

namespace n_transitive {

struct X {
  X() : next() {}
  copp::memory::strong_rc_ptr<X> next;
};

void test() {
  copp::memory::strong_rc_ptr<X> p(new X);
  p->next = copp::memory::strong_rc_ptr<X>(new X);
  CASE_EXPECT_TRUE(!p->next->next);
  p = p->next;
  CASE_EXPECT_TRUE(!p->next);
}

}  // namespace n_transitive

namespace n_report_1 {

class foo {
 public:
  foo() : m_self(this) {}

  void suicide() { m_self.reset(); }

 private:
  copp::memory::strong_rc_ptr<foo> m_self;
};

void test() {
  foo *foo_ptr = new foo;
  foo_ptr->suicide();
}

}  // namespace n_report_1

// Test case by Per Kristensen
namespace n_report_2 {

class foo {
 public:
  void setWeak(copp::memory::strong_rc_ptr<foo> s) { w = s; }

 private:
  copp::memory::weak_rc_ptr<foo> w;
};

class deleter {
 public:
  deleter() : lock(0) {}

  ~deleter() { CASE_EXPECT_TRUE(lock == 0); }

  void operator()(foo *p) {
    ++lock;
    delete p;
    --lock;
  }

 private:
  int lock;
};

void test() {
  copp::memory::strong_rc_ptr<foo> s(new foo, deleter());
  s->setWeak(s);
  s.reset();
}

}  // namespace n_report_2

namespace n_spt_incomplete {

class file;

copp::memory::strong_rc_ptr<file> fopen(char const *name, char const *mode);
void fread(copp::memory::strong_rc_ptr<file> f, void *data, int64_t size);

int file_instances = 0;

void test() {
  CASE_EXPECT_TRUE(file_instances == 0);

  {
    copp::memory::strong_rc_ptr<file> pf = fopen("name", "mode");
    CASE_EXPECT_TRUE(file_instances == 1);
    fread(pf, 0, 17041);
  }

  CASE_EXPECT_TRUE(file_instances == 0);
}

}  // namespace n_spt_incomplete

namespace n_spt_pimpl {

class file {
 private:
  class impl;
  copp::memory::strong_rc_ptr<impl> pimpl_;

 public:
  file(char const *name, char const *mode);

  // compiler generated members are fine and useful

  void read(void *data, int64_t size);

  int64_t total_size() const;
};

int file_instances = 0;

void test() {
  CASE_EXPECT_TRUE(file_instances == 0);

  {
    file f("name", "mode");
    CASE_EXPECT_TRUE(file_instances == 1);
    f.read(0, 152);

    file f2(f);
    CASE_EXPECT_TRUE(file_instances == 1);
    f2.read(0, 894);

    CASE_EXPECT_TRUE(f.total_size() == 152 + 894);

    {
      file f3("name2", "mode2");
      CASE_EXPECT_TRUE(file_instances == 2);
    }

    CASE_EXPECT_TRUE(file_instances == 1);
  }

  CASE_EXPECT_TRUE(file_instances == 0);
}

}  // namespace n_spt_pimpl

namespace n_spt_abstract {

class X {
 public:
  virtual void f(int) = 0;
  virtual int g() = 0;

 protected:
  virtual ~X() {}
};

copp::memory::strong_rc_ptr<X> createX();

int X_instances = 0;

void test() {
  CASE_EXPECT_TRUE(X_instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = createX();

    CASE_EXPECT_TRUE(X_instances == 1);

    px->f(18);
    px->f(152);

    CASE_EXPECT_TRUE(px->g() == 170);
  }

  CASE_EXPECT_TRUE(X_instances == 0);
}

}  // namespace n_spt_abstract

namespace n_spt_preventing_delete {

int X_instances = 0;

class X {
 private:
  X() { ++X_instances; }

  ~X() { --X_instances; }

  class deleter;
  friend class deleter;

  class deleter {
   public:
    void operator()(X *p) { delete p; }
  };

 public:
  static copp::memory::strong_rc_ptr<X> create() {
    copp::memory::strong_rc_ptr<X> px(new X, X::deleter());
    return px;
  }
};

void test() {
  CASE_EXPECT_TRUE(X_instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = X::create();
    CASE_EXPECT_TRUE(X_instances == 1);
  }

  CASE_EXPECT_TRUE(X_instances == 0);
}

}  // namespace n_spt_preventing_delete

namespace n_spt_array {

int X_instances = 0;

struct X {
  X() { ++X_instances; }

  ~X() { --X_instances; }
};

void test() {
  CASE_EXPECT_TRUE(X_instances == 0);

  {
    auto p = new X[4];
    copp::memory::strong_rc_ptr<X> px(p, checked_array_deleter<X>());
    CASE_EXPECT_TRUE(X_instances == 4);
  }

  CASE_EXPECT_TRUE(X_instances == 0);
}

}  // namespace n_spt_array

namespace n_spt_static {

class X {
 public:
  X() {}

 private:
  void operator delete(void *) {
    // Comeau 4.3.0.1 wants a definition
    CASE_EXPECT_ERROR("n_spt_static::X::operator delete() called.");
  }
};

struct null_deleter {
  void operator()(void const *) const {}
};

static X x;

void test() { copp::memory::strong_rc_ptr<X> px(&x, null_deleter()); }

}  // namespace n_spt_static

namespace n_spt_intrusive {

int X_instances = 0;

struct X {
  int64_t count;

  X() : count(0) { ++X_instances; }

  ~X() { --X_instances; }
};

void intrusive_ptr_add_ref(X *p) { ++p->count; }

void intrusive_ptr_release(X *p) {
  if (--p->count == 0) delete p;
}

template <class T>
struct intrusive_deleter {
  void operator()(T *p) {
    if (p != 0) intrusive_ptr_release(p);
  }
};

copp::memory::strong_rc_ptr<X> make_shared_from_intrusive(X *p) {
  if (p != 0) intrusive_ptr_add_ref(p);
  copp::memory::strong_rc_ptr<X> px(p, intrusive_deleter<X>());
  return px;
}

void test() {
  CASE_EXPECT_TRUE(X_instances == 0);

  {
    X *p = new X;
    CASE_EXPECT_TRUE(X_instances == 1);
    CASE_EXPECT_TRUE(p->count == 0);
    copp::memory::strong_rc_ptr<X> px = make_shared_from_intrusive(p);
    CASE_EXPECT_TRUE(px.get() == p);
    CASE_EXPECT_TRUE(p->count == 1);
    copp::memory::strong_rc_ptr<X> px2(px);
    CASE_EXPECT_TRUE(px2.get() == p);
    CASE_EXPECT_TRUE(p->count == 1);
  }

  CASE_EXPECT_TRUE(X_instances == 0);
}

}  // namespace n_spt_intrusive

namespace n_spt_another_sp {

template <class T>
class another_ptr : private copp::memory::strong_rc_ptr<T> {
 private:
  typedef copp::memory::strong_rc_ptr<T> base_type;

 public:
  explicit another_ptr(T *p = 0) : base_type(p) {}

  void reset() { base_type::reset(); }

  T *get() const { return base_type::get(); }
};

class event_handler {
 public:
  virtual ~event_handler() {}
  virtual void begin() = 0;
  virtual void handle(int event) = 0;
  virtual void end() = 0;
};

int begin_called = 0;
int handle_called = 0;
int end_called = 0;

class event_handler_impl : public event_handler {
 public:
  virtual void begin() { ++begin_called; }

  virtual void handle(int event) { handle_called = event; }

  virtual void end() { ++end_called; }
};

another_ptr<event_handler> get_event_handler() {
  another_ptr<event_handler> p(new event_handler_impl);
  return p;
}

copp::memory::strong_rc_ptr<event_handler> current_handler;

void install_event_handler(copp::memory::strong_rc_ptr<event_handler> p) {
  p->begin();
  current_handler = p;
}

void handle_event(int event) { current_handler->handle(event); }

void remove_event_handler() {
  current_handler->end();
  current_handler.reset();
}

template <class P>
class smart_pointer_deleter {
 private:
  P p_;

 public:
  explicit smart_pointer_deleter(P const &p) : p_(p) {}

  void operator()(void const *) { p_.reset(); }
};

void test() {
  another_ptr<event_handler> p = get_event_handler();

  copp::memory::strong_rc_ptr<event_handler> q(p.get(), smart_pointer_deleter<another_ptr<event_handler>>(p));

  p.reset();

  CASE_EXPECT_TRUE(begin_called == 0);

  install_event_handler(q);

  CASE_EXPECT_TRUE(begin_called == 1);

  CASE_EXPECT_TRUE(handle_called == 0);

  handle_event(17041);

  CASE_EXPECT_TRUE(handle_called == 17041);

  CASE_EXPECT_TRUE(end_called == 0);

  remove_event_handler();

  CASE_EXPECT_TRUE(end_called == 1);
}

}  // namespace n_spt_another_sp

namespace n_spt_shared_from_this {

class X {
 public:
  virtual void f() = 0;

 protected:
  virtual ~X() {}
};

class Y {
 public:
  virtual copp::memory::strong_rc_ptr<X> getX() = 0;

 protected:
  virtual ~Y() {}
};

class impl : public X, public Y {
 private:
  copp::memory::weak_rc_ptr<impl> weak_this;

  impl(impl const &);
  impl &operator=(impl const &);

  impl() {}

 public:
  static copp::memory::strong_rc_ptr<impl> create() {
    copp::memory::strong_rc_ptr<impl> pi(new impl);
    pi->weak_this = pi;
    return pi;
  }

  virtual void f() {}

  virtual copp::memory::strong_rc_ptr<X> getX() {
    copp::memory::strong_rc_ptr<X> px = weak_this.lock();
    return px;
  }
};

void test() {
  copp::memory::strong_rc_ptr<Y> py = impl::create();
  CASE_EXPECT_TRUE(py.get() != 0);
  CASE_EXPECT_TRUE(py.use_count() == 1);

  copp::memory::strong_rc_ptr<X> px = py->getX();
  CASE_EXPECT_TRUE(px.get() != 0);
  CASE_EXPECT_TRUE(py.use_count() == 2);

#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
  copp::memory::strong_rc_ptr<Y> py2 = copp::memory::dynamic_pointer_cast<Y>(px);
  CASE_EXPECT_TRUE(py.get() == py2.get());
  CASE_EXPECT_TRUE(!(py < py2 || py2 < py));
  CASE_EXPECT_TRUE(py.use_count() == 3);
#endif
}

}  // namespace n_spt_shared_from_this

namespace n_spt_wrap {

void test() {}

}  // namespace n_spt_wrap

CASE_TEST(rc_ptr, strong_rc_basic) {
  n_element_type::test();
  n_constructors::test();
  n_assignment::test();
  n_reset::test();
  n_access::test();
  n_use_count::test();
  n_swap::test();
  n_comparison::test();
  n_static_cast::test();
  n_const_cast::test();
#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
  n_dynamic_cast::test();
#endif

  n_map::test();

  n_transitive::test();
  n_report_1::test();
  n_report_2::test();

  n_spt_incomplete::test();
  n_spt_pimpl::test();
  n_spt_abstract::test();
  n_spt_preventing_delete::test();
  n_spt_array::test();
  n_spt_static::test();
  n_spt_intrusive::test();
  n_spt_another_sp::test();
  n_spt_shared_from_this::test();
  n_spt_wrap::test();
}

namespace n_spt_incomplete {

class file {
 public:
  file() : fread_called(false) { ++file_instances; }

  ~file() {
    CASE_EXPECT_TRUE(fread_called);
    --file_instances;
  }

  bool fread_called;
};

copp::memory::strong_rc_ptr<file> fopen(char const *, char const *) {
  copp::memory::strong_rc_ptr<file> pf(new file);
  return pf;
}

void fread(copp::memory::strong_rc_ptr<file> pf, void *, int64_t) { pf->fread_called = true; }

}  // namespace n_spt_incomplete

namespace n_spt_pimpl {

class file::impl {
 private:
  impl(impl const &);
  impl &operator=(impl const &);

  int64_t total_size_;

 public:
  impl(char const *, char const *) : total_size_(0) { ++file_instances; }

  ~impl() { --file_instances; }

  void read(void *, int64_t size) { total_size_ += size; }

  int64_t total_size() const { return total_size_; }
};

file::file(char const *name, char const *mode) : pimpl_(new impl(name, mode)) {}

void file::read(void *data, int64_t size) { pimpl_->read(data, size); }

int64_t file::total_size() const { return pimpl_->total_size(); }

}  // namespace n_spt_pimpl

namespace n_spt_abstract {

class X_impl : public X {
 private:
  X_impl(X_impl const &);
  X_impl &operator=(X_impl const &);

  int n_;

 public:
  X_impl() : n_(0) { ++X_instances; }

  ~X_impl() { --X_instances; }

  virtual void f(int n) { n_ += n; }

  virtual int g() { return n_; }
};

copp::memory::strong_rc_ptr<X> createX() {
  copp::memory::strong_rc_ptr<X> px(new X_impl);
  return px;
}

}  // namespace n_spt_abstract
}  // namespace strong_rc

namespace strong_rc_fn {

static void f() {}

struct null_deleter {
  template <class Y>
  void operator()(Y *) {}
};

CASE_TEST(rc_ptr, strong_rc_fn) {
  copp::memory::strong_rc_ptr<void()> pf(f, null_deleter());

  CASE_EXPECT_EQ(f, pf.get());

  copp::memory::weak_rc_ptr<void()> wp(pf);

  CASE_EXPECT_EQ(wp.lock().get(), f);
  CASE_EXPECT_EQ(wp.use_count(), 1);

  pf.reset();

  CASE_EXPECT_EQ(wp.lock().get(), nullptr);
  CASE_EXPECT_EQ(wp.use_count(), 0);
}
}  // namespace strong_rc_fn

namespace strong_rc_move {

struct X {
  static int64_t instances;

  X() { ++instances; }

  ~X() { --instances; }

 private:
  X(X const &);
  X &operator=(X const &);
};

int64_t X::instances = 0;

CASE_TEST(rc_ptr, strong_rc_move) {
  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    copp::memory::strong_rc_ptr<X> p2(std::move(p));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    copp::memory::strong_rc_ptr<void> p3(std::move(p2));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p2.get() == 0);

    p3.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    copp::memory::strong_rc_ptr<X> p2;
    p2 = std::move(p);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    copp::memory::strong_rc_ptr<void> p3;
    p3 = std::move(p2);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p2.get() == 0);

    p3.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    copp::memory::strong_rc_ptr<X> p2(new X);
    CASE_EXPECT_TRUE(X::instances == 2);
    p2 = std::move(p);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    copp::memory::strong_rc_ptr<void> p3(new X);
    CASE_EXPECT_TRUE(X::instances == 2);
    p3 = std::move(p2);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p2.get() == 0);

    p3.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
  }
}

}  // namespace strong_rc_move

namespace make_strong_rc {

class X {
 private:
  X(X const &);
  X &operator=(X const &);

  void *operator new(std::size_t n) {
    // lack of this definition causes link errors on Comeau C++
    CASE_EXPECT_ERROR("private X::new called");
    return ::operator new(n);
  }

  void operator delete(void *p) {
    // lack of this definition causes link errors on MSVC
    CASE_EXPECT_ERROR("private X::delete called");
    ::operator delete(p);
  }

 public:
  static int instances;

  int v;

  explicit X(int a1 = 0, int a2 = 0, int a3 = 0, int a4 = 0, int a5 = 0, int a6 = 0, int a7 = 0, int a8 = 0, int a9 = 0)
      : v(a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9) {
    ++instances;
  }

  ~X() { --instances; }
};

int X::instances = 0;

CASE_TEST(rc_ptr, make_strong_rc) {
  {
    copp::memory::strong_rc_ptr<int> pi = copp::memory::make_strong_rc<int>();

    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);

    CASE_EXPECT_TRUE(*pi == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> pi = copp::memory::make_strong_rc<int>(5);

    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);

    CASE_EXPECT_TRUE(*pi == 5);
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>();
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 0);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3, 4);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3, 4, 5);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3, 4, 5, 6);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3, 4, 5, 6, 7);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6 + 7);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3, 4, 5, 6, 7, 8);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::make_strong_rc<X>(1, 2, 3, 4, 5, 6, 7, 8, 9);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi.use_count() == 1);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }
}
}  // namespace make_strong_rc

namespace weak_rc {

namespace n_element_type {

void f(int &) {}

void test() {
  typedef copp::memory::weak_rc_ptr<int>::element_type T;
  T t;
  f(t);
}

}  // namespace n_element_type

class incomplete;

copp::memory::strong_rc_ptr<incomplete> create_incomplete();

struct X {
  int dummy;
};

struct Y {
  int dummy2;
};

struct Z : public X, public virtual Y {};

namespace n_constructors {

void default_constructor() {
  {
    copp::memory::weak_rc_ptr<int> wp;
    CASE_EXPECT_TRUE(wp.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<void> wp;
    CASE_EXPECT_TRUE(wp.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<incomplete> wp;
    CASE_EXPECT_TRUE(wp.use_count() == 0);
  }
}

void shared_ptr_constructor() {
  {
    copp::memory::strong_rc_ptr<int> sp;

    copp::memory::weak_rc_ptr<int> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());

    copp::memory::weak_rc_ptr<void> wp2(sp);
    CASE_EXPECT_TRUE(wp2.use_count() == sp.use_count());
  }

  {
    copp::memory::strong_rc_ptr<int> sp(static_cast<int *>(0));

    {
      copp::memory::weak_rc_ptr<int> wp(sp);
      CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
      CASE_EXPECT_TRUE(wp.use_count() == 1);
      copp::memory::strong_rc_ptr<int> sp2(wp);
      CASE_EXPECT_TRUE(wp.use_count() == 2);
      CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
    }

    {
      copp::memory::weak_rc_ptr<void> wp(sp);
      CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
      CASE_EXPECT_TRUE(wp.use_count() == 1);
      copp::memory::strong_rc_ptr<void> sp2(wp);
      CASE_EXPECT_TRUE(wp.use_count() == 2);
      CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
    }
  }

  {
    copp::memory::strong_rc_ptr<int> sp(new int);

    {
      copp::memory::weak_rc_ptr<int> wp(sp);
      CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
      CASE_EXPECT_TRUE(wp.use_count() == 1);
      copp::memory::strong_rc_ptr<int> sp2(wp);
      CASE_EXPECT_TRUE(wp.use_count() == 2);
      CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
    }

    {
      copp::memory::weak_rc_ptr<void> wp(sp);
      CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
      CASE_EXPECT_TRUE(wp.use_count() == 1);
      copp::memory::strong_rc_ptr<void> sp2(wp);
      CASE_EXPECT_TRUE(wp.use_count() == 2);
      CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
    }
  }

  {
    copp::memory::strong_rc_ptr<void> sp;

    copp::memory::weak_rc_ptr<void> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
  }

  {
    copp::memory::strong_rc_ptr<void> sp(static_cast<int *>(0));

    copp::memory::weak_rc_ptr<void> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
    CASE_EXPECT_TRUE(wp.use_count() == 1);
    copp::memory::strong_rc_ptr<void> sp2(wp);
    CASE_EXPECT_TRUE(wp.use_count() == 2);
    CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
  }

  {
    copp::memory::strong_rc_ptr<void> sp(new int);

    copp::memory::weak_rc_ptr<void> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
    CASE_EXPECT_TRUE(wp.use_count() == 1);
    copp::memory::strong_rc_ptr<void> sp2(wp);
    CASE_EXPECT_TRUE(wp.use_count() == 2);
    CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
  }

  {
    copp::memory::strong_rc_ptr<incomplete> sp;

    copp::memory::weak_rc_ptr<incomplete> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());

    copp::memory::weak_rc_ptr<void> wp2(sp);
    CASE_EXPECT_TRUE(wp2.use_count() == sp.use_count());
  }

  {
    copp::memory::strong_rc_ptr<incomplete> sp = create_incomplete();

    {
      copp::memory::weak_rc_ptr<incomplete> wp(sp);
      CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
      CASE_EXPECT_TRUE(wp.use_count() == 1);
      copp::memory::strong_rc_ptr<incomplete> sp2(wp);
      CASE_EXPECT_TRUE(wp.use_count() == 2);
      CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
    }

    {
      copp::memory::weak_rc_ptr<void> wp(sp);
      CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
      CASE_EXPECT_TRUE(wp.use_count() == 1);
      copp::memory::strong_rc_ptr<void> sp2(wp);
      CASE_EXPECT_TRUE(wp.use_count() == 2);
      CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
    }
  }

  {
    copp::memory::strong_rc_ptr<void> sp = create_incomplete();

    copp::memory::weak_rc_ptr<void> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == sp.use_count());
    CASE_EXPECT_TRUE(wp.use_count() == 1);
    copp::memory::strong_rc_ptr<void> sp2(wp);
    CASE_EXPECT_TRUE(wp.use_count() == 2);
    CASE_EXPECT_TRUE(!(sp < sp2 || sp2 < sp));
  }
}

void copy_constructor() {
  {
    copp::memory::weak_rc_ptr<int> wp;
    copp::memory::weak_rc_ptr<int> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<void> wp;
    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<incomplete> wp;
    copp::memory::weak_rc_ptr<incomplete> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> sp(static_cast<int *>(0));
    copp::memory::weak_rc_ptr<int> wp(sp);

    copp::memory::weak_rc_ptr<int> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<int> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<int> sp(new int);
    copp::memory::weak_rc_ptr<int> wp(sp);

    copp::memory::weak_rc_ptr<int> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<int> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<void> sp(static_cast<int *>(0));
    copp::memory::weak_rc_ptr<void> wp(sp);

    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<void> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<void> sp(new int);
    copp::memory::weak_rc_ptr<void> wp(sp);

    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<void> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<incomplete> sp = create_incomplete();
    copp::memory::weak_rc_ptr<incomplete> wp(sp);

    copp::memory::weak_rc_ptr<incomplete> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<incomplete> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }
}

void conversion_constructor() {
  {
    copp::memory::weak_rc_ptr<int> wp;
    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<incomplete> wp;
    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<Z> wp;

    copp::memory::weak_rc_ptr<X> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);

    copp::memory::weak_rc_ptr<Y> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> sp(static_cast<int *>(0));
    copp::memory::weak_rc_ptr<int> wp(sp);

    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<void> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<int> sp(new int);
    copp::memory::weak_rc_ptr<int> wp(sp);

    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<void> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<incomplete> sp = create_incomplete();
    copp::memory::weak_rc_ptr<incomplete> wp(sp);

    copp::memory::weak_rc_ptr<void> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<void> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<Z> sp(static_cast<Z *>(0));
    copp::memory::weak_rc_ptr<Z> wp(sp);

    copp::memory::weak_rc_ptr<X> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<X> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<Z> sp(static_cast<Z *>(0));
    copp::memory::weak_rc_ptr<Z> wp(sp);

    copp::memory::weak_rc_ptr<Y> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<Y> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<Z> sp(new Z);
    copp::memory::weak_rc_ptr<Z> wp(sp);

    copp::memory::weak_rc_ptr<X> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<X> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }

  {
    copp::memory::strong_rc_ptr<Z> sp(new Z);
    copp::memory::weak_rc_ptr<Z> wp(sp);

    copp::memory::weak_rc_ptr<Y> wp2(wp);
    CASE_EXPECT_TRUE(wp2.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    sp.reset();
    // CASE_EXPECT_TRUE(!(wp < wp2 || wp2 < wp));

    copp::memory::weak_rc_ptr<Y> wp3(wp);
    CASE_EXPECT_TRUE(wp3.use_count() == wp.use_count());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));
  }
}

void test() {
  default_constructor();
  shared_ptr_constructor();
  copy_constructor();
  conversion_constructor();
}

}  // namespace n_constructors

namespace n_assignment {

template <class T>
void copy_assignment(copp::memory::strong_rc_ptr<T> &sp) {
  CASE_EXPECT_TRUE(sp.unique());

  copp::memory::weak_rc_ptr<T> p1;

  p1 = p1;
  CASE_EXPECT_TRUE(p1.use_count() == 0);

  copp::memory::weak_rc_ptr<T> p2;

  p1 = p2;
  CASE_EXPECT_TRUE(p1.use_count() == 0);

  copp::memory::weak_rc_ptr<T> p3(p1);

  p1 = p3;
  CASE_EXPECT_TRUE(p1.use_count() == 0);

  copp::memory::weak_rc_ptr<T> p4(sp);

  p4 = p4;
  CASE_EXPECT_TRUE(p4.use_count() == 1);

  p1 = p4;
  CASE_EXPECT_TRUE(p1.use_count() == 1);

  p4 = p2;
  CASE_EXPECT_TRUE(p4.use_count() == 0);

  sp.reset();

  p1 = p1;
  CASE_EXPECT_TRUE(p1.use_count() == 0);

  p4 = p1;
  CASE_EXPECT_TRUE(p4.use_count() == 0);
}

void conversion_assignment() {
  {
    copp::memory::weak_rc_ptr<void> p1;

    copp::memory::weak_rc_ptr<incomplete> p2;

    p1 = p2;
    CASE_EXPECT_TRUE(p1.use_count() == 0);

    copp::memory::strong_rc_ptr<incomplete> sp = create_incomplete();
    copp::memory::weak_rc_ptr<incomplete> p3(sp);

    p1 = p3;
    CASE_EXPECT_TRUE(p1.use_count() == 1);

    sp.reset();

    p1 = p3;
    CASE_EXPECT_TRUE(p1.use_count() == 0);

    p1 = p2;
    CASE_EXPECT_TRUE(p1.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<X> p1;

    copp::memory::weak_rc_ptr<Z> p2;

    p1 = p2;
    CASE_EXPECT_TRUE(p1.use_count() == 0);

    copp::memory::strong_rc_ptr<Z> sp(new Z);
    copp::memory::weak_rc_ptr<Z> p3(sp);

    p1 = p3;
    CASE_EXPECT_TRUE(p1.use_count() == 1);

    sp.reset();

    p1 = p3;
    CASE_EXPECT_TRUE(p1.use_count() == 0);

    p1 = p2;
    CASE_EXPECT_TRUE(p1.use_count() == 0);
  }

  {
    copp::memory::weak_rc_ptr<Y> p1;

    copp::memory::weak_rc_ptr<Z> p2;

    p1 = p2;
    CASE_EXPECT_TRUE(p1.use_count() == 0);

    copp::memory::strong_rc_ptr<Z> sp(new Z);
    copp::memory::weak_rc_ptr<Z> p3(sp);

    p1 = p3;
    CASE_EXPECT_TRUE(p1.use_count() == 1);

    sp.reset();

    p1 = p3;
    CASE_EXPECT_TRUE(p1.use_count() == 0);

    p1 = p2;
    CASE_EXPECT_TRUE(p1.use_count() == 0);
  }
}

template <class T, class U>
void shared_ptr_assignment(copp::memory::strong_rc_ptr<U> &sp, T * = 0) {
  CASE_EXPECT_TRUE(sp.unique());

  copp::memory::weak_rc_ptr<T> p1;
  copp::memory::weak_rc_ptr<T> p2(p1);
  copp::memory::weak_rc_ptr<T> p3(sp);
  copp::memory::weak_rc_ptr<T> p4(p3);

  p1 = sp;
  CASE_EXPECT_TRUE(p1.use_count() == 1);

  p2 = sp;
  CASE_EXPECT_TRUE(p2.use_count() == 1);

  p3 = sp;
  CASE_EXPECT_TRUE(p3.use_count() == 1);

  p4 = sp;
  CASE_EXPECT_TRUE(p4.use_count() == 1);

  sp.reset();

  CASE_EXPECT_TRUE(p1.use_count() == 0);
  CASE_EXPECT_TRUE(p2.use_count() == 0);
  CASE_EXPECT_TRUE(p3.use_count() == 0);
  CASE_EXPECT_TRUE(p4.use_count() == 0);

  p1 = sp;
}

void test() {
  {
    copp::memory::strong_rc_ptr<int> p(new int);
    copy_assignment(p);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    copy_assignment(p);
  }

  {
    copp::memory::strong_rc_ptr<void> p(new int);
    copy_assignment(p);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> p = create_incomplete();
    copy_assignment(p);
  }

  conversion_assignment();

  {
    copp::memory::strong_rc_ptr<int> p(new int);
    shared_ptr_assignment<int>(p);
  }

  {
    copp::memory::strong_rc_ptr<int> p(new int);
    shared_ptr_assignment<void>(p);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    shared_ptr_assignment<X>(p);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    shared_ptr_assignment<void>(p);
  }

  {
    copp::memory::strong_rc_ptr<void> p(new int);
    shared_ptr_assignment<void>(p);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> p = create_incomplete();
    shared_ptr_assignment<incomplete>(p);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> p = create_incomplete();
    shared_ptr_assignment<void>(p);
  }
}

}  // namespace n_assignment

namespace n_reset {

template <class T, class U>
void test2(copp::memory::strong_rc_ptr<U> &sp, T * = 0) {
  CASE_EXPECT_TRUE(sp.unique());

  copp::memory::weak_rc_ptr<T> p1;
  copp::memory::weak_rc_ptr<T> p2(p1);
  copp::memory::weak_rc_ptr<T> p3(sp);
  copp::memory::weak_rc_ptr<T> p4(p3);
  copp::memory::weak_rc_ptr<T> p5(sp);
  copp::memory::weak_rc_ptr<T> p6(p5);

  p1.reset();
  CASE_EXPECT_TRUE(p1.use_count() == 0);

  p2.reset();
  CASE_EXPECT_TRUE(p2.use_count() == 0);

  p3.reset();
  CASE_EXPECT_TRUE(p3.use_count() == 0);

  p4.reset();
  CASE_EXPECT_TRUE(p4.use_count() == 0);

  sp.reset();

  p5.reset();
  CASE_EXPECT_TRUE(p5.use_count() == 0);

  p6.reset();
  CASE_EXPECT_TRUE(p6.use_count() == 0);
}

void test() {
  {
    copp::memory::strong_rc_ptr<int> p(new int);
    test2<int>(p);
  }

  {
    copp::memory::strong_rc_ptr<int> p(new int);
    test2<void>(p);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    test2<X>(p);
  }

  {
    copp::memory::strong_rc_ptr<X> p(new X);
    test2<void>(p);
  }

  {
    copp::memory::strong_rc_ptr<void> p(new int);
    test2<void>(p);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> p = create_incomplete();
    test2<incomplete>(p);
  }

  {
    copp::memory::strong_rc_ptr<incomplete> p = create_incomplete();
    test2<void>(p);
  }
}

}  // namespace n_reset

namespace n_use_count {

void test() {
  {
    copp::memory::weak_rc_ptr<X> wp;
    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp.expired());

    copp::memory::weak_rc_ptr<X> wp2;
    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp.expired());

    copp::memory::weak_rc_ptr<X> wp3(wp);
    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    CASE_EXPECT_TRUE(wp3.expired());
  }

  {
    copp::memory::strong_rc_ptr<X> sp(static_cast<X *>(0));

    copp::memory::weak_rc_ptr<X> wp(sp);
    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(!wp.expired());

    copp::memory::weak_rc_ptr<X> wp2(sp);
    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(!wp.expired());

    copp::memory::weak_rc_ptr<X> wp3(wp);
    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(!wp.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 1);
    CASE_EXPECT_TRUE(!wp3.expired());

    copp::memory::strong_rc_ptr<X> sp2(sp);

    CASE_EXPECT_TRUE(wp.use_count() == 2);
    CASE_EXPECT_TRUE(!wp.expired());
    CASE_EXPECT_TRUE(wp2.use_count() == 2);
    CASE_EXPECT_TRUE(!wp2.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 2);
    CASE_EXPECT_TRUE(!wp3.expired());

    copp::memory::strong_rc_ptr<void> sp3(sp);

    CASE_EXPECT_TRUE(wp.use_count() == 3);
    CASE_EXPECT_TRUE(!wp.expired());
    CASE_EXPECT_TRUE(wp2.use_count() == 3);
    CASE_EXPECT_TRUE(!wp2.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 3);
    CASE_EXPECT_TRUE(!wp3.expired());

    sp.reset();

    CASE_EXPECT_TRUE(wp.use_count() == 2);
    CASE_EXPECT_TRUE(!wp.expired());
    CASE_EXPECT_TRUE(wp2.use_count() == 2);
    CASE_EXPECT_TRUE(!wp2.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 2);
    CASE_EXPECT_TRUE(!wp3.expired());

    sp2.reset();

    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(!wp.expired());
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    CASE_EXPECT_TRUE(!wp2.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 1);
    CASE_EXPECT_TRUE(!wp3.expired());

    sp3.reset();

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp.expired());
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.expired());
    CASE_EXPECT_TRUE(wp3.use_count() == 0);
    CASE_EXPECT_TRUE(wp3.expired());
  }
}

}  // namespace n_use_count

namespace n_swap {

void test() {
  {
    copp::memory::weak_rc_ptr<X> wp;
    copp::memory::weak_rc_ptr<X> wp2;

    wp.swap(wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);

    using std::swap;
    swap(wp, wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> sp(new X);
    copp::memory::weak_rc_ptr<X> wp;
    copp::memory::weak_rc_ptr<X> wp2(sp);
    copp::memory::weak_rc_ptr<X> wp3(sp);

    wp.swap(wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));

    using std::swap;
    swap(wp, wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp2 < wp3 || wp3 < wp2));

    sp.reset();

    wp.swap(wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));

    swap(wp, wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp2 < wp3 || wp3 < wp2));
  }

  {
    copp::memory::strong_rc_ptr<X> sp(new X);
    copp::memory::strong_rc_ptr<X> sp2(new X);
    copp::memory::weak_rc_ptr<X> wp(sp);
    copp::memory::weak_rc_ptr<X> wp2(sp2);
    copp::memory::weak_rc_ptr<X> wp3(sp2);

    wp.swap(wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));

    using std::swap;
    swap(wp, wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp2 < wp3 || wp3 < wp2));

    sp.reset();

    wp.swap(wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 1);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));

    swap(wp, wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 1);
    // CASE_EXPECT_TRUE(!(wp2 < wp3 || wp3 < wp2));

    sp2.reset();

    wp.swap(wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp < wp3 || wp3 < wp));

    swap(wp, wp2);

    CASE_EXPECT_TRUE(wp.use_count() == 0);
    CASE_EXPECT_TRUE(wp2.use_count() == 0);
    // CASE_EXPECT_TRUE(!(wp2 < wp3 || wp3 < wp2));
  }
}

}  // namespace n_swap

namespace n_lock {

void test() {}

}  // namespace n_lock

CASE_TEST(rc_ptr, weak_rc_ptr_basic) {
  n_element_type::test();
  n_constructors::test();
  n_assignment::test();
  n_reset::test();
  n_use_count::test();
  n_swap::test();
  n_lock::test();
}

class incomplete {};

copp::memory::strong_rc_ptr<incomplete> create_incomplete() {
  copp::memory::strong_rc_ptr<incomplete> px(new incomplete);
  return px;
}

}  // namespace weak_rc

namespace weak_rc_ptr_move {

struct X {
  static int64_t instances;

  X() { ++instances; }

  ~X() { --instances; }

 private:
  X(X const &);
  X &operator=(X const &);
};

int64_t X::instances = 0;

CASE_TEST(rc_ptr, weak_rc_ptr_move) {
  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> p_(new X);
    copp::memory::weak_rc_ptr<X> p(p_);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.use_count() == 1);

    copp::memory::weak_rc_ptr<X> p2(std::move(p));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p2.use_count() == 1);
    CASE_EXPECT_TRUE(p.expired());

    copp::memory::weak_rc_ptr<void> p3(std::move(p2));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p3.use_count() == 1);
    CASE_EXPECT_TRUE(p2.expired());

    p_.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(p3.expired());
  }

  {
    copp::memory::strong_rc_ptr<X> p_(new X);
    copp::memory::weak_rc_ptr<X> p(p_);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.use_count() == 1);

    copp::memory::weak_rc_ptr<X> p2;
    p2 = static_cast<copp::memory::weak_rc_ptr<X> &&>(p);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p2.use_count() == 1);
    CASE_EXPECT_TRUE(p.expired());

    copp::memory::weak_rc_ptr<void> p3;
    p3 = std::move(p2);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p3.use_count() == 1);
    CASE_EXPECT_TRUE(p2.expired());

    p_.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
    CASE_EXPECT_TRUE(p3.expired());
  }

  {
    copp::memory::strong_rc_ptr<X> p_(new X);
    copp::memory::weak_rc_ptr<X> p(p_);
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.use_count() == 1);

    copp::memory::strong_rc_ptr<X> p_2(new X);
    copp::memory::weak_rc_ptr<X> p2(p_2);
    CASE_EXPECT_TRUE(X::instances == 2);
    p2 = std::move(p);
    CASE_EXPECT_TRUE(X::instances == 2);
    CASE_EXPECT_TRUE(p2.use_count() == 1);
    CASE_EXPECT_TRUE(p.expired());
    CASE_EXPECT_TRUE(p2.lock() != p_2);

    copp::memory::strong_rc_ptr<void> p_3(new X);
    copp::memory::weak_rc_ptr<void> p3(p_3);
    CASE_EXPECT_TRUE(X::instances == 3);
    p3 = std::move(p2);
    CASE_EXPECT_TRUE(X::instances == 3);
    CASE_EXPECT_TRUE(p3.use_count() == 1);
    CASE_EXPECT_TRUE(p2.expired());
    CASE_EXPECT_TRUE(p3.lock() != p_3);
  }
}
}  // namespace weak_rc_ptr_move

template <class L, class R>
static bool owner_before_comp(const L &l, const R &r) noexcept {
  return l.owner_before(r);
}

CASE_TEST(rc_ptr, owner_before) {
  {
    copp::memory::strong_rc_ptr<int> x;
    copp::memory::strong_rc_ptr<int> y;
    copp::memory::weak_rc_ptr<int> w;
    CASE_EXPECT_TRUE(!(owner_before_comp(x, w) || owner_before_comp(w, x)));
  }
  {
    copp::memory::strong_rc_ptr<int> z(reinterpret_cast<int *>(0));
    copp::memory::weak_rc_ptr<int> w;
    CASE_EXPECT_TRUE(owner_before_comp(z, w) || owner_before_comp(w, z));
    {
      copp::memory::strong_rc_ptr<int> zz(z);
      w = copp::memory::weak_rc_ptr<int>(zz);
      CASE_EXPECT_TRUE(!(owner_before_comp(z, zz) || owner_before_comp(z, zz)));
      CASE_EXPECT_TRUE(!(owner_before_comp(z, w) || owner_before_comp(z, w)));
    }
    CASE_EXPECT_TRUE(!(owner_before_comp(z, w) || owner_before_comp(w, z)));
  }
  {
    copp::memory::strong_rc_ptr<int> x;
    copp::memory::strong_rc_ptr<int> z(reinterpret_cast<int *>(0));
    CASE_EXPECT_TRUE(owner_before_comp(x, z) || owner_before_comp(z, x));
  }
  {
    copp::memory::strong_rc_ptr<int> a(reinterpret_cast<int *>(0));
    copp::memory::strong_rc_ptr<int> b(reinterpret_cast<int *>(0));
    CASE_EXPECT_TRUE(owner_before_comp(a, b) || owner_before_comp(b, a));
    copp::memory::weak_rc_ptr<int> w(a);
    CASE_EXPECT_TRUE(!(owner_before_comp(a, w) || owner_before_comp(w, a)));
    CASE_EXPECT_TRUE(owner_before_comp(b, w) || owner_before_comp(w, b));
  }

  {
    copp::memory::strong_rc_ptr<int> a(reinterpret_cast<int *>(0));
    copp::memory::weak_rc_ptr<int> wa(a);
    copp::memory::strong_rc_ptr<int> b(reinterpret_cast<int *>(0));
    copp::memory::weak_rc_ptr<int> wb(b);
    CASE_EXPECT_TRUE(!(owner_before_comp(a, wa) || owner_before_comp(wa, a)));
    CASE_EXPECT_TRUE(!(owner_before_comp(b, wb) || owner_before_comp(wb, b)));
    CASE_EXPECT_TRUE(owner_before_comp(wa, wb) || owner_before_comp(wb, wa));
    CASE_EXPECT_TRUE(owner_before_comp(wa, b) || owner_before_comp(b, wa));
  }
}

template <class L, class R>
static bool owner_equal_comp(const L &l, const R &r) noexcept {
  return l.owner_equal(r);
}

CASE_TEST(rc_ptr, owner_equal) {
  {
    copp::memory::strong_rc_ptr<int> p1(new int);
    copp::memory::strong_rc_ptr<int> p2(p1);

    CASE_EXPECT_TRUE(owner_equal_comp(p1, p2));
    CASE_EXPECT_TRUE(owner_equal_comp(p2, p1));

    copp::memory::strong_rc_ptr<int> p3(new int);

    CASE_EXPECT_TRUE(!owner_equal_comp(p1, p3));
    CASE_EXPECT_TRUE(!owner_equal_comp(p3, p1));

    copp::memory::strong_rc_ptr<int> p4;
    copp::memory::strong_rc_ptr<int> p5;

    CASE_EXPECT_TRUE(owner_equal_comp(p4, p5));
    CASE_EXPECT_TRUE(owner_equal_comp(p5, p4));

    CASE_EXPECT_TRUE(!owner_equal_comp(p4, p3));
    CASE_EXPECT_TRUE(!owner_equal_comp(p3, p4));

    copp::memory::strong_rc_ptr<int> p6(static_cast<int *>(0));

    CASE_EXPECT_TRUE(!owner_equal_comp(p4, p6));
    CASE_EXPECT_TRUE(!owner_equal_comp(p6, p4));

    copp::memory::strong_rc_ptr<void> p7(p1);

    CASE_EXPECT_TRUE(owner_equal_comp(p1, p7));
    CASE_EXPECT_TRUE(owner_equal_comp(p7, p1));

    copp::memory::strong_rc_ptr<void> p8;

    CASE_EXPECT_TRUE(!owner_equal_comp(p1, p8));
    CASE_EXPECT_TRUE(!owner_equal_comp(p8, p1));

    CASE_EXPECT_TRUE(owner_equal_comp(p4, p8));
    CASE_EXPECT_TRUE(owner_equal_comp(p8, p4));

    copp::memory::weak_rc_ptr<int> q1(p1);

    CASE_EXPECT_TRUE(owner_equal_comp(p1, q1));
    CASE_EXPECT_TRUE(owner_equal_comp(q1, p1));

    copp::memory::weak_rc_ptr<int> q2(p1);

    CASE_EXPECT_TRUE(owner_equal_comp(q1, q2));
    CASE_EXPECT_TRUE(owner_equal_comp(q2, q1));

    copp::memory::weak_rc_ptr<int> q3(p3);

    CASE_EXPECT_TRUE(!owner_equal_comp(p1, q3));
    CASE_EXPECT_TRUE(!owner_equal_comp(q3, p1));

    CASE_EXPECT_TRUE(!owner_equal_comp(q1, q3));
    CASE_EXPECT_TRUE(!owner_equal_comp(q3, q1));

    copp::memory::weak_rc_ptr<int> q4;

    CASE_EXPECT_TRUE(owner_equal_comp(p4, q4));
    CASE_EXPECT_TRUE(owner_equal_comp(q4, p4));

    CASE_EXPECT_TRUE(!owner_equal_comp(q1, q4));
    CASE_EXPECT_TRUE(!owner_equal_comp(q4, q1));

    copp::memory::weak_rc_ptr<void> q5;

    CASE_EXPECT_TRUE(owner_equal_comp(q4, q5));
    CASE_EXPECT_TRUE(owner_equal_comp(q5, q4));

    copp::memory::weak_rc_ptr<void> q7(p7);

    CASE_EXPECT_TRUE(owner_equal_comp(p1, q7));
    CASE_EXPECT_TRUE(owner_equal_comp(q7, p1));

    CASE_EXPECT_TRUE(owner_equal_comp(q1, q7));
    CASE_EXPECT_TRUE(owner_equal_comp(q7, q1));

    p1.reset();
    p2.reset();
    p3.reset();
    p7.reset();

    CASE_EXPECT_TRUE(q1.expired());
    CASE_EXPECT_TRUE(q2.expired());
    CASE_EXPECT_TRUE(q3.expired());
    CASE_EXPECT_TRUE(q7.expired());

    CASE_EXPECT_TRUE(owner_equal_comp(q1, q2));
    CASE_EXPECT_TRUE(owner_equal_comp(q2, q1));

    CASE_EXPECT_TRUE(owner_equal_comp(q1, q7));
    CASE_EXPECT_TRUE(owner_equal_comp(q7, q1));

    CASE_EXPECT_TRUE(!owner_equal_comp(q1, q3));
    CASE_EXPECT_TRUE(!owner_equal_comp(q3, q1));

    CASE_EXPECT_TRUE(!owner_equal_comp(q1, q4));
    CASE_EXPECT_TRUE(!owner_equal_comp(q4, q1));
  }
}

template <class P>
static std::size_t get_owner_hash(const P &l) noexcept {
  return l.owner_hash();
}

CASE_TEST(rc_ptr, owner_hash) {
  copp::memory::strong_rc_ptr<int> p1(new int);
  copp::memory::strong_rc_ptr<int> p2(p1);

  CASE_EXPECT_EQ(get_owner_hash(p1), get_owner_hash(p2));

  copp::memory::strong_rc_ptr<int> p3(new int);

  CASE_EXPECT_NE(get_owner_hash(p1), get_owner_hash(p3));

  copp::memory::strong_rc_ptr<int> p4;
  copp::memory::strong_rc_ptr<int> p5;

  CASE_EXPECT_EQ(get_owner_hash(p4), get_owner_hash(p5));
  CASE_EXPECT_NE(get_owner_hash(p4), get_owner_hash(p3));

  copp::memory::strong_rc_ptr<int> p6(static_cast<int *>(0));

  CASE_EXPECT_NE(get_owner_hash(p4), get_owner_hash(p6));

  copp::memory::strong_rc_ptr<void> p7(p1);

  CASE_EXPECT_EQ(get_owner_hash(p1), get_owner_hash(p7));

  copp::memory::strong_rc_ptr<void> p8;

  CASE_EXPECT_NE(get_owner_hash(p1), get_owner_hash(p8));
  CASE_EXPECT_EQ(get_owner_hash(p4), get_owner_hash(p8));

  copp::memory::weak_rc_ptr<int> q1(p1);

  CASE_EXPECT_EQ(get_owner_hash(p1), get_owner_hash(q1));

  copp::memory::weak_rc_ptr<int> q2(p1);

  CASE_EXPECT_EQ(get_owner_hash(q1), get_owner_hash(q2));

  copp::memory::weak_rc_ptr<int> q3(p3);

  CASE_EXPECT_NE(get_owner_hash(p1), get_owner_hash(q3));
  CASE_EXPECT_NE(get_owner_hash(q1), get_owner_hash(q3));

  copp::memory::weak_rc_ptr<int> q4;

  CASE_EXPECT_EQ(get_owner_hash(p4), get_owner_hash(q4));
  CASE_EXPECT_NE(get_owner_hash(q1), get_owner_hash(q4));

  copp::memory::weak_rc_ptr<void> q5;

  CASE_EXPECT_EQ(get_owner_hash(q4), get_owner_hash(q5));

  copp::memory::weak_rc_ptr<void> q7(p7);

  CASE_EXPECT_EQ(get_owner_hash(p1), get_owner_hash(q7));
  CASE_EXPECT_EQ(get_owner_hash(q1), get_owner_hash(q7));

  p1.reset();
  p2.reset();
  p3.reset();
  p7.reset();

  CASE_EXPECT_TRUE(q1.expired());
  CASE_EXPECT_TRUE(q2.expired());
  CASE_EXPECT_TRUE(q3.expired());
  CASE_EXPECT_TRUE(q7.expired());

  CASE_EXPECT_EQ(get_owner_hash(q1), get_owner_hash(q2));
  CASE_EXPECT_EQ(get_owner_hash(q1), get_owner_hash(q7));
  CASE_EXPECT_NE(get_owner_hash(q1), get_owner_hash(q3));
  CASE_EXPECT_NE(get_owner_hash(q1), get_owner_hash(q4));
}

namespace shared_from_this {

class X {
 public:
  virtual void f() = 0;

 protected:
  virtual ~X() {}
};

class Y {
 public:
  virtual copp::memory::strong_rc_ptr<X> getX() = 0;

 protected:
  virtual ~Y() {}
};

copp::memory::strong_rc_ptr<Y> createY();

void test() {
  copp::memory::strong_rc_ptr<Y> py = createY();
  CASE_EXPECT_TRUE(py.get() != 0);
  CASE_EXPECT_TRUE(py.use_count() == 1);

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
  try {
    copp::memory::strong_rc_ptr<X> px = py->getX();
    CASE_EXPECT_TRUE(px.get() != 0);
    CASE_EXPECT_TRUE(py.use_count() == 2);

    px->f();

#  if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
    copp::memory::strong_rc_ptr<Y> py2 = copp::memory::dynamic_pointer_cast<Y>(px);
    CASE_EXPECT_TRUE(py.get() == py2.get());
    CASE_EXPECT_TRUE(!(py < py2 || py2 < py));
    CASE_EXPECT_TRUE(py.use_count() == 3);
#  endif
  } catch (std::bad_weak_ptr const &) {
    CASE_EXPECT_ERROR("py->getX() failed");
  }
#endif
}

void test2();
void test3();

CASE_TEST(rc_ptr, shared_from_this) {
  test();
  test2();
  test3();
}

// virtual inheritance to stress the implementation
// (prevents Y* -> impl*, enable_shared_rc_from_this<impl>* -> impl* casts)

class impl : public X, public virtual Y, public virtual copp::memory::enable_shared_rc_from_this<impl> {
 public:
  virtual void f() {}

  virtual copp::memory::strong_rc_ptr<X> getX() {
    copp::memory::strong_rc_ptr<impl> pi = shared_from_this();
    CASE_EXPECT_TRUE(pi.get() == this);
    return pi;
  }
};

// intermediate impl2 to stress the implementation

class impl2 : public impl {};

copp::memory::strong_rc_ptr<Y> createY() {
  copp::memory::strong_rc_ptr<Y> pi(new impl2);
  return pi;
}

void test2() { copp::memory::strong_rc_ptr<Y> pi(static_cast<impl2 *>(0)); }

//

struct V : public copp::memory::enable_shared_rc_from_this<V> {};

void test3() {
  copp::memory::strong_rc_ptr<V> p(new V);

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
  try {
    copp::memory::strong_rc_ptr<V> q = p->shared_from_this();
    CASE_EXPECT_TRUE(p == q);
    CASE_EXPECT_TRUE(!(p < q) && !(q < p));
  } catch (std::bad_weak_ptr const &) {
    CASE_EXPECT_ERROR("p->shared_from_this() failed");
  }

  V v2(*p);

  try {
    copp::memory::strong_rc_ptr<V> r = v2.shared_from_this();
    CASE_EXPECT_ERROR("v2.shared_from_this() failed to throw");
  } catch (std::bad_weak_ptr const &) {
  }

  try {
    *p = V();
    copp::memory::strong_rc_ptr<V> r = p->shared_from_this();
    CASE_EXPECT_ERROR("*p = V() failed to throw");
  } catch (std::bad_weak_ptr const &) {
  }
#endif
}

}  // namespace shared_from_this

namespace sp_unique_ptr {
struct X : public ::copp::memory::enable_shared_rc_from_this<X> {
  static int instances;

  X() { ++instances; }

  ~X() { --instances; }

 private:
  X(X const &);
  X &operator=(X const &);
};

int X::instances = 0;

struct Y {
  static int instances;

  bool deleted_;

  Y() : deleted_(false) { ++instances; }

  ~Y() {
    CASE_EXPECT_TRUE(deleted_);
    --instances;
  }

 private:
  Y(Y const &);
  Y &operator=(Y const &);
};

int Y::instances = 0;

struct YD {
  void operator()(Y *p) const {
    if (p) {
      p->deleted_ = true;
      delete p;
    } else {
      CASE_EXPECT_ERROR("YD::operator()(0) called");
    }
  }
};

template <class U, class T, class D>
static void test_null_unique_ptr(std::unique_ptr<T, D> p1, std::unique_ptr<T, D> p2) {
  CASE_EXPECT_TRUE(T::instances == 0);

  ::copp::memory::strong_rc_ptr<U> sp(std::move(p1));

  CASE_EXPECT_TRUE(sp.get() == 0);
  CASE_EXPECT_TRUE(sp.use_count() == 0);

  sp.reset(new T, typename std::remove_reference<D>::type());

  CASE_EXPECT_TRUE(sp.get() != 0);
  CASE_EXPECT_TRUE(sp.use_count() == 1);

  CASE_EXPECT_TRUE(T::instances == 1);

  sp = std::move(p2);

  CASE_EXPECT_TRUE(sp.get() == 0);
  CASE_EXPECT_TRUE(sp.use_count() == 0);

  CASE_EXPECT_TRUE(T::instances == 0);
}

CASE_TEST(rc_ptr, sp_unique_ptr) {
  {
    CASE_EXPECT_TRUE(X::instances == 0);

    std::unique_ptr<X> p(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    ::copp::memory::strong_rc_ptr<X> p2(std::move(p));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    ::copp::memory::strong_rc_ptr<X> p3 = p2->shared_from_this();
    CASE_EXPECT_TRUE(p2 == p3);
    CASE_EXPECT_TRUE(!(p2 < p3) && !(p3 < p2));

    p2.reset();
    p3.reset();
    CASE_EXPECT_TRUE(X::instances == 0);

    p2 = std::unique_ptr<X>(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    p2 = std::unique_ptr<X>(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    p2.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    CASE_EXPECT_TRUE(X::instances == 0);

    std::unique_ptr<X> p(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    ::copp::memory::strong_rc_ptr<X const> p2(std::move(p));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    ::copp::memory::strong_rc_ptr<X const> p3 = p2->shared_from_this();
    CASE_EXPECT_TRUE(p2 == p3);
    CASE_EXPECT_TRUE(!(p2 < p3) && !(p3 < p2));

    p2.reset();
    p3.reset();
    CASE_EXPECT_TRUE(X::instances == 0);

    p2 = std::unique_ptr<X>(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    p2 = std::unique_ptr<X>(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    p2.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    CASE_EXPECT_TRUE(X::instances == 0);

    std::unique_ptr<X> p(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    ::copp::memory::strong_rc_ptr<void> p2(std::move(p));
    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    p2.reset();
    CASE_EXPECT_TRUE(X::instances == 0);

    p2 = std::unique_ptr<X>(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    p2 = std::unique_ptr<X>(new X);
    CASE_EXPECT_TRUE(X::instances == 1);

    p2.reset();
    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    CASE_EXPECT_TRUE(Y::instances == 0);

    std::unique_ptr<Y, YD> p(new Y, YD());
    CASE_EXPECT_TRUE(Y::instances == 1);

    ::copp::memory::strong_rc_ptr<Y> p2(std::move(p));
    CASE_EXPECT_TRUE(Y::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    p2.reset();
    CASE_EXPECT_TRUE(Y::instances == 0);

    p2 = std::unique_ptr<Y, YD>(new Y, YD());
    CASE_EXPECT_TRUE(Y::instances == 1);

    p2 = std::unique_ptr<Y, YD>(new Y, YD());
    CASE_EXPECT_TRUE(Y::instances == 1);

    p2.reset();
    CASE_EXPECT_TRUE(Y::instances == 0);
  }

  {
    CASE_EXPECT_TRUE(Y::instances == 0);

    YD yd;

    std::unique_ptr<Y, YD &> p(new Y, yd);
    CASE_EXPECT_TRUE(Y::instances == 1);

    ::copp::memory::strong_rc_ptr<Y> p2(std::move(p));
    CASE_EXPECT_TRUE(Y::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    p2.reset();
    CASE_EXPECT_TRUE(Y::instances == 0);

    p2 = std::unique_ptr<Y, YD &>(new Y, yd);
    CASE_EXPECT_TRUE(Y::instances == 1);

    p2 = std::unique_ptr<Y, YD &>(new Y, yd);
    CASE_EXPECT_TRUE(Y::instances == 1);

    p2.reset();
    CASE_EXPECT_TRUE(Y::instances == 0);
  }

  {
    CASE_EXPECT_TRUE(Y::instances == 0);

    YD yd;

    std::unique_ptr<Y, YD const &> p(new Y, yd);
    CASE_EXPECT_TRUE(Y::instances == 1);

    ::copp::memory::strong_rc_ptr<Y> p2(std::move(p));
    CASE_EXPECT_TRUE(Y::instances == 1);
    CASE_EXPECT_TRUE(p.get() == 0);

    p2.reset();
    CASE_EXPECT_TRUE(Y::instances == 0);

    p2 = std::unique_ptr<Y, YD const &>(new Y, yd);
    CASE_EXPECT_TRUE(Y::instances == 1);

    p2 = std::unique_ptr<Y, YD const &>(new Y, yd);
    CASE_EXPECT_TRUE(Y::instances == 1);

    p2.reset();
    CASE_EXPECT_TRUE(Y::instances == 0);
  }

  {
    test_null_unique_ptr<X>(std::unique_ptr<X>(), std::unique_ptr<X>());
    test_null_unique_ptr<X const>(std::unique_ptr<X>(), std::unique_ptr<X>());
    test_null_unique_ptr<void>(std::unique_ptr<X>(), std::unique_ptr<X>());
    test_null_unique_ptr<void const>(std::unique_ptr<X>(), std::unique_ptr<X>());
  }

  {
    test_null_unique_ptr<Y>(std::unique_ptr<Y, YD>(0, YD()), std::unique_ptr<Y, YD>(0, YD()));
    test_null_unique_ptr<Y const>(std::unique_ptr<Y, YD>(0, YD()), std::unique_ptr<Y, YD>(0, YD()));
    test_null_unique_ptr<void>(std::unique_ptr<Y, YD>(0, YD()), std::unique_ptr<Y, YD>(0, YD()));
    test_null_unique_ptr<void const>(std::unique_ptr<Y, YD>(0, YD()), std::unique_ptr<Y, YD>(0, YD()));
  }

  {
    YD yd;

    test_null_unique_ptr<Y>(std::unique_ptr<Y, YD &>(0, yd), std::unique_ptr<Y, YD &>(0, yd));
    test_null_unique_ptr<Y const>(std::unique_ptr<Y, YD &>(0, yd), std::unique_ptr<Y, YD &>(0, yd));
    test_null_unique_ptr<void>(std::unique_ptr<Y, YD &>(0, yd), std::unique_ptr<Y, YD &>(0, yd));
    test_null_unique_ptr<void const>(std::unique_ptr<Y, YD &>(0, yd), std::unique_ptr<Y, YD &>(0, yd));
  }
}
}  // namespace sp_unique_ptr

CASE_TEST(rc_ptr, compat) {
  static_assert(
      std::is_same<copp::memory::compat_strong_ptr_type_trait<std::string,
                                                              copp::memory::compat_strong_ptr_mode::kStl>::shared_ptr,
                   std::shared_ptr<std::string>>::value,
      "check shared_ptr failed");
  static_assert(std::is_same<copp::memory::compat_strong_ptr_type_trait<
                                 std::string, copp::memory::compat_strong_ptr_mode::kStrongRc>::shared_ptr,
                             copp::memory::strong_rc_ptr<std::string>>::value,
                "check strong_rc_ptr failed");

  static_assert(
      std::is_same<
          copp::memory::compat_strong_ptr_type_trait<std::string, copp::memory::compat_strong_ptr_mode::kStl>::weak_ptr,
          std::weak_ptr<std::string>>::value,
      "check weak_ptr failed");
  static_assert(std::is_same<copp::memory::compat_strong_ptr_type_trait<
                                 std::string, copp::memory::compat_strong_ptr_mode::kStrongRc>::weak_ptr,
                             copp::memory::weak_rc_ptr<std::string>>::value,
                "check strong_rc_ptr failed");

  static_assert(std::is_same<copp::memory::compat_strong_ptr_function_trait<
                                 copp::memory::compat_strong_ptr_mode::kStl>::enable_shared_from_this<std::string>,
                             std::enable_shared_from_this<std::string>>::value,
                "check enable_shared_from_this failed");
  static_assert(std::is_same<copp::memory::compat_strong_ptr_function_trait<
                                 copp::memory::compat_strong_ptr_mode::kStrongRc>::enable_shared_from_this<std::string>,
                             copp::memory::enable_shared_rc_from_this<std::string>>::value,
                "check enable_shared_from_this failed");

  static_assert(std::is_same<decltype(copp::memory::compat_strong_ptr_function_trait<
                                      copp::memory::compat_strong_ptr_mode::kStl>::make_shared<std::string>()),
                             std::shared_ptr<std::string>>::value,
                "check make_shared failed");
  static_assert(std::is_same<decltype(copp::memory::compat_strong_ptr_function_trait<
                                      copp::memory::compat_strong_ptr_mode::kStrongRc>::make_shared<std::string>()),
                             copp::memory::strong_rc_ptr<std::string>>::value,
                "check make_shared failed");
}

namespace allocate_strong_rc_test {
class X {
 private:
  X(X const &);
  X &operator=(X const &);

  void *operator new(std::size_t n) {
    // lack of this definition causes link errors on Comeau C++
    CASE_EXPECT_ERROR("private X::new called");
    return ::operator new(n);
  }

  void operator delete(void *p) {
    // lack of this definition causes link errors on MSVC
    CASE_EXPECT_ERROR("private X::delete called");
    ::operator delete(p);
  }

 public:
  static int instances;

  int v;

  explicit X(int a1 = 0, int a2 = 0, int a3 = 0, int a4 = 0, int a5 = 0, int a6 = 0, int a7 = 0, int a8 = 0, int a9 = 0)
      : v(a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9) {
    ++instances;
  }

  ~X() { --instances; }
};

int X::instances = 0;

CASE_TEST(rc_ptr, allocate_strong_rc_test) {
  {
    copp::memory::strong_rc_ptr<int> pi = copp::memory::allocate_strong_rc<int>(std::allocator<int>());

    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(*pi == 0);
  }

  {
    copp::memory::strong_rc_ptr<int> pi = copp::memory::allocate_strong_rc<int>(std::allocator<int>(), 5);

    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(*pi == 5);
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>());
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 0);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi =
        copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6, 7);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6 + 7);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi =
        copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6, 7, 8);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }

  {
    copp::memory::strong_rc_ptr<X> pi =
        copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
    copp::memory::weak_rc_ptr<X> wp(pi);

    CASE_EXPECT_TRUE(X::instances == 1);
    CASE_EXPECT_TRUE(pi.get() != 0);
    CASE_EXPECT_TRUE(pi->v == 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);

    pi.reset();

    CASE_EXPECT_TRUE(X::instances == 0);
  }
}
}  // namespace allocate_strong_rc_test

namespace allocate_strong_rc_esft_test {
class X : public copp::memory::enable_shared_rc_from_this<X> {
 private:
  X(X const &);
  X &operator=(X const &);

 public:
  static int instances;

  explicit X(int = 0, int = 0, int = 0, int = 0, int = 0, int = 0, int = 0, int = 0, int = 0) { ++instances; }

  ~X() { --instances; }
};

int X::instances = 0;

CASE_TEST(rc_ptr, allocate_strong_rc_esft_test) {
  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>());
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px = copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px =
        copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6, 7);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px =
        copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6, 7, 8);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);

  {
    copp::memory::strong_rc_ptr<X> px =
        copp::memory::allocate_strong_rc<X>(std::allocator<void>(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
    CASE_EXPECT_TRUE(X::instances == 1);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
      copp::memory::strong_rc_ptr<X> qx = px->shared_from_this();

      CASE_EXPECT_TRUE(px == qx);
      CASE_EXPECT_TRUE(!(px < qx) && !(qx < px));

      px.reset();
      CASE_EXPECT_TRUE(X::instances == 1);
    } catch (std::bad_weak_ptr const &) {
      CASE_EXPECT_ERROR("px->shared_from_this() failed");
    }
#endif
  }

  CASE_EXPECT_TRUE(X::instances == 0);
}
}  // namespace allocate_strong_rc_esft_test

#if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic pop
#  endif
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic pop
#endif
