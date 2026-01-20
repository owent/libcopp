// Copyright 2026 owent

#include <type_traits>

#include "frame/test_macros.h"

#include "libcopp/utils/memory/rc_ptr.h"
#include "libcopp/utils/nostd/nullability.h"
#include "libcopp/utils/nostd/type_traits.h"

CASE_TEST(nostd_nullability, nullable) {
#if (defined(__cplusplus) && __cplusplus >= 201703L) && (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
  {
    constexpr auto nullability_compatible_test = copp::nostd::__is_nullability_compatible<void>::value;
    CASE_EXPECT_FALSE(nullability_compatible_test);
  }
#endif

  {
    constexpr auto nullable_test = std::is_same<copp::nostd::nullable<void*>, void*>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test = std::is_same<copp::nostd::nullable<const char*>, const char*>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test = std::is_same<copp::nostd::nullable<void (*)()>, void (*)()>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test =
        std::is_same<copp::nostd::nullable<std::shared_ptr<int>>, std::shared_ptr<int>>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test =
        std::is_same<copp::nostd::nullable<std::unique_ptr<int>>, std::unique_ptr<int>>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test =
        std::is_same<copp::nostd::nullable<copp::memory::strong_rc_ptr<int>>, copp::memory::strong_rc_ptr<int>>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }
}

CASE_TEST(nostd_nullability, nonnull) {
  {
    constexpr auto nullable_test = std::is_same<copp::nostd::nonnull<void*>, void*>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test = std::is_same<copp::nostd::nonnull<const char*>, const char*>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test = std::is_same<copp::nostd::nonnull<void (*)()>, void (*)()>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test =
        std::is_same<copp::nostd::nonnull<std::shared_ptr<int>>, std::shared_ptr<int>>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test =
        std::is_same<copp::nostd::nonnull<std::unique_ptr<int>>, std::unique_ptr<int>>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }

  {
    constexpr auto nullable_test =
        std::is_same<copp::nostd::nonnull<copp::memory::strong_rc_ptr<int>>, copp::memory::strong_rc_ptr<int>>::value;
    CASE_EXPECT_TRUE(nullable_test);
  }
}

namespace test {
struct test_nostd_type_traits_a {
  using type1 = int;
};

struct test_nostd_type_traits_b {
  LIBCOPP_UTIL_NOSTD_TYPE_TRAITS_CONDITION_NESTED_TYPE_AS_MEMBER(test_nostd_type_traits_a, type1, type1, bool);
  LIBCOPP_UTIL_NOSTD_TYPE_TRAITS_CONDITION_NESTED_TYPE_AS_MEMBER(test_nostd_type_traits_a, type2, type2, bool);
};
}  // namespace test

CASE_TEST(nostd_type_traits, detected_or) {}
