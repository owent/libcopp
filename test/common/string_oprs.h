/**
 * @file string_oprs.h
 * @brief 字符串相关操作
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author owent
 * @date 2015.11.24
 *
 * @history
 *
 *
 */

#ifndef UTIL_COMMON_STRING_OPRS_H
#define UTIL_COMMON_STRING_OPRS_H

// 目测主流编译器都支持且有优化， gcc 3.4 and upper, vc, clang, c++ builder xe3, intel c++ and etc.
#pragma once

#include <stdint.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ostream>

#if defined(_MSC_VER) && _MSC_VER >= 1600
#  define UTIL_STRFUNC_STRCASE_CMP(l, r) _stricmp(l, r)
#  define UTIL_STRFUNC_STRNCASE_CMP(l, r, s) _strnicmp(l, r, s)
#  define UTIL_STRFUNC_STRCMP(l, r) strcmp(l, r)
#  define UTIL_STRFUNC_STRNCMP(l, r, s) strncmp(l, r, s)
#else
#  define UTIL_STRFUNC_STRCASE_CMP(l, r) strcasecmp(l, r)
#  define UTIL_STRFUNC_STRNCASE_CMP(l, r, s) strncasecmp(l, r, s)
#  define UTIL_STRFUNC_STRCMP(l, r) strcmp(l, r)
#  define UTIL_STRFUNC_STRNCMP(l, r, s) strncmp(l, r, s)
#endif

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    defined(__STDC_LIB_EXT1__)
#  define UTIL_STRFUNC_SSCANF(...) sscanf_s(__VA_ARGS__)

#  ifdef _MSC_VER
#    define UTIL_STRFUNC_VSNPRINTF(buffer, bufsz, fmt, arg) \
      vsnprintf_s(buffer, static_cast<size_t>(bufsz), _TRUNCATE, fmt, arg)
#    define UTIL_STRFUNC_SNPRINTF(...) sprintf_s(__VA_ARGS__)
#  else
#    define UTIL_STRFUNC_VSNPRINTF(buffer, bufsz, fmt, arg) vsnprintf_s(buffer, static_cast<size_t>(bufsz), fmt, arg)
#    define UTIL_STRFUNC_SNPRINTF(...) snprintf_s(__VA_ARGS__)
#  endif

#  define UTIL_STRFUNC_C11_SUPPORT 1
#else
#  define UTIL_STRFUNC_SSCANF(...) sscanf(__VA_ARGS__)
#  define UTIL_STRFUNC_SNPRINTF(...) snprintf(__VA_ARGS__)
#  define UTIL_STRFUNC_VSNPRINTF(buffer, bufsz, fmt, arg) vsnprintf(buffer, static_cast<int>(bufsz), fmt, arg)
#endif

namespace util {
namespace string {
/**
 * @brief 字符转小写
 * @param c 字符
 * @return str 如果是大写字符输出响应的小写字符，否则原样返回
 * @note 用于替换标准函数里参数是int类型导致的某些编译器warning问题
 */
template <typename TCH = char>
static TCH tolower(TCH c) {
  if (c >= 'A' && c <= 'Z') {
    return static_cast<TCH>(c - 'A' + 'a');
  }

  return c;
}

/**
 * @brief 字符转大写
 * @param c 字符
 * @return str 如果是小写字符输出响应的大写字符，否则原样返回
 * @note 用于替换标准函数里参数是int类型导致的某些编译器warning问题
 */
template <typename TCH = char>
static TCH toupper(TCH c) {
  if (c >= 'a' && c <= 'z') {
    return static_cast<TCH>(c - 'a' + 'A');
  }

  return c;
}

/**
 * @brief 字符串转整数
 * @param out 输出的整数
 * @param str 被转换的字符串
 * @note 性能肯定比sscanf系，和iostream系高。strtol系就不知道了
 */
template <typename T>
void str2int(T &out, const char *str) {
  out = static_cast<T>(0);
  if (nullptr == str || !(*str)) {
    return;
  }

  // negative
  bool is_negative = false;
  while (*str && *str == '-') {
    is_negative = !is_negative;
    ++str;
  }

  if (!(*str)) {
    return;
  }

  if ('0' == str[0] && 'x' == tolower(str[1])) {  // hex
    for (size_t i = 2; str[i]; ++i) {
      char c = tolower(str[i]);
      if (c >= '0' && c <= '9') {
        out <<= 4;
        out += c - '0';
      } else if (c >= 'a' && c <= 'f') {
        out <<= 4;
        out += c - 'a' + 10;
      } else {
        break;
      }
    }
  } else if ('\\' == str[0]) {  // oct
    for (size_t i = 1; str[i] >= '0' && str[i] < '8'; ++i) {
      out <<= 3;
      out += str[i] - '0';
    }
  } else {  // dec
    for (size_t i = 0; str[i] >= '0' && str[i] <= '9'; ++i) {
      out *= 10;
      out += str[i] - '0';
    }
  }

  if (is_negative) {
    out = (~out) + 1;
  }
}

/**
 * @brief 字符串转整数
 * @param str 被转换的字符串
 * @return 输出的整数
 */
template <typename T>
inline T to_int(const char *str) {
  T ret = 0;
  str2int(ret, str);
  return ret;
}

/**
 * @brief 字符转十六进制表示
 * @param out 输出的字符串(缓冲区长度至少为2)
 * @param c 被转换的字符
 * @param upper_case 输出大写字符？
 */
template <typename TStr, typename TCh>
void hex(TStr *out, TCh c, bool upper_case = false) {
  out[0] = static_cast<TStr>((c >> 4) & 0x0F);
  out[1] = static_cast<TStr>(c & 0x0F);

  for (int i = 0; i < 2; ++i) {
    if (out[i] > 9) {
      out[i] += (upper_case ? 'A' : 'a') - 10;
    } else {
      out[i] += '0';
    }
  }
}

/**
 * @brief 字符转8进制表示
 * @param out 输出的字符串(缓冲区长度至少为3)
 * @param c 被转换的字符
 * @param upper_case 输出大写字符？
 */
template <typename TStr, typename TCh>
void oct(TStr *out, TCh c) {
  out[0] = static_cast<TStr>(((c >> 6) & 0x07) + '0');
  out[1] = static_cast<TStr>(((c >> 3) & 0x07) + '0');
  out[2] = static_cast<TStr>((c & 0x07) + '0');
}

/**
 * @brief 字符转8进制表示
 * @param src 输入的buffer
 * @param ss 输入的buffer长度
 * @param out 输出buffer
 * @param os 输出buffer长度，回传输出缓冲区使用的长度
 */
template <typename TCh>
void serialization(const void *src, size_t ss, TCh *out, size_t &os) {
  const TCh *cs = reinterpret_cast<const TCh *>(src);
  size_t i, j;
  for (i = 0, j = 0; i < ss && j < os; ++i) {
    if (cs[i] >= 32 && cs[i] < 127) {
      out[j] = cs[i];
      ++j;
    } else if (j + 4 <= os) {
      out[j++] = '\\';
      oct(&out[j], cs[i]);
      j += 3;
    } else {
      break;
    }
  }

  os = j;
}

/**
 * @brief 字符转8进制表示
 * @param src 输入的buffer
 * @param ss 输入的buffer长度
 * @param out 输出缓冲区
 */
template <typename Elem, typename Traits>
void serialization(const void *src, size_t ss, std::basic_ostream<Elem, Traits> &out) {
  const Elem *cs = reinterpret_cast<const Elem *>(src);
  size_t i;
  for (i = 0; i < ss; ++i) {
    if (cs[i] >= 32 && cs[i] < 127) {
      out.put(cs[i]);
    } else {
      Elem tmp[4] = {'\\', 0, 0, 0};
      oct(&tmp[1], cs[i]);
      out.write(tmp, 4);
    }
  }
}

/**
 * @brief 字符转16进制表示
 * @param src 输入的buffer
 * @param ss 输入的buffer长度
 * @param out 输出buffer
 * @param upper_case 是否大写
 */
template <typename TCh>
void dumphex(const void *src, size_t ss, TCh *out, bool upper_case = false) {
  const unsigned char *cs = reinterpret_cast<const unsigned char *>(src);
  size_t i;
  for (i = 0; i < ss; ++i) {
    hex<TCh, unsigned char>(&out[i << 1], cs[i], upper_case);
  }
}

/**
 * @brief 字符转16进制表示
 * @param src 输入的buffer
 * @param ss 输入的buffer长度
 * @param out 输出缓冲区
 * @param upper_case 是否大写
 */
template <typename Elem, typename Traits>
void dumphex(const void *src, size_t ss, std::basic_ostream<Elem, Traits> &out, bool upper_case = false) {
  const unsigned char *cs = reinterpret_cast<const unsigned char *>(src);
  size_t i;
  Elem tmp[2];
  for (i = 0; i < ss; ++i) {
    hex<Elem, unsigned char>(tmp, cs[i], upper_case);
    out.write(tmp, 2);
  }
}
}  // namespace string
}  // namespace util

#endif /* _UTIL_COMMON_COMPILER_MESSAGE_H_ */
