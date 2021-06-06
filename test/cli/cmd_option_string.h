#ifndef UTIL_CLI_CMDOPTION_STRING_H
#define UTIL_CLI_CMDOPTION_STRING_H

#pragma once

/**
 * 特殊指令字符串类
 */

#include <algorithm>
#include <string>

namespace util {
namespace cli {
/**
 * 功能受限的字符串类型处理类
 */
template <typename Tc>
struct ci_char_traits : public std::char_traits<Tc> {
  static bool eq(Tc left, Tc right) { return toupper(left) == toupper(right); }
  static bool lt(Tc left, Tc right) { return toupper(left) < toupper(right); }

  static int compare(const Tc *left, const Tc *right, size_t n) {
    while (n-- > 0) {
      char cl = (Tc)toupper(*left), cr = (Tc)toupper(*right);
      if (cl < cr)
        return -1;
      else if (cl > cr)
        return 1;

      ++left, ++right;
    }
    return 0;
  }

  static const Tc *find(const char *s, int n, Tc a) {
    while (n-- > 0 && toupper(*s) != toupper(a)) ++s;
    return n >= 0 ? s : 0;
  }
};

// 类型重定义
typedef std::basic_string<char, ci_char_traits<char> > cmd_option_ci_string;
}  // namespace cli
}  // namespace util
#endif
