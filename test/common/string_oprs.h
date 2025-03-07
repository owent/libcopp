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

#pragma once

#ifndef __STDC_WANT_LIB_EXT1__
#  define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <stdint.h>

// Import the C++20 feature-test macros
#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  endif
#elif defined(_MSC_VER) && \
    ((defined(__cplusplus) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L))
#  if _MSC_VER >= 1922
#    include <version>
#  endif
#endif

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#ifdef __cpp_lib_string_view
#  include <string_view>
#endif

#include <type_traits>

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
#    define UTIL_STRFUNC_SNPRINTF(buffer, bufsz, ...) sprintf_s(buffer, static_cast<size_t>(bufsz), __VA_ARGS__)
#  else
#    define UTIL_STRFUNC_VSNPRINTF(buffer, bufsz, fmt, arg) vsnprintf_s(buffer, static_cast<rsize_t>(bufsz), fmt, arg)
#    define UTIL_STRFUNC_SNPRINTF(buffer, bufsz, fmt, args...) \
      snprintf_s(buffer, static_cast<rsize_t>(bufsz), fmt, ##args)
#  endif

#  define UTIL_STRFUNC_C11_SUPPORT 1
#else
#  define UTIL_STRFUNC_SSCANF(...) sscanf(__VA_ARGS__)
#  define UTIL_STRFUNC_SNPRINTF(buffer, bufsz, fmt, args...) snprintf(buffer, static_cast<size_t>(bufsz), fmt, ##args)
#  define UTIL_STRFUNC_VSNPRINTF(buffer, bufsz, fmt, arg) vsnprintf(buffer, static_cast<size_t>(bufsz), fmt, arg)
#endif

namespace util {
namespace string {
/**
 * @brief 字符转小写
 * @param c 字符
 * @return str 如果是大写字符输出响应的小写字符，否则原样返回
 * @note 用于替换标准函数里参数是int类型导致的某些编译器warning问题
 */
template <class TCH = char>
TCH tolower(TCH c) {
  if (c >= 'A' && c <= 'Z') {
    return static_cast<TCH>(c + static_cast<TCH>('a' - 'A'));
  }

  return c;
}

/**
 * @brief 字符转大写
 * @param c 字符
 * @return str 如果是小写字符输出响应的大写字符，否则原样返回
 * @note 用于替换标准函数里参数是int类型导致的某些编译器warning问题
 */
template <class TCH = char>
TCH toupper(TCH c) {
  if (c >= 'a' && c <= 'z') {
    return static_cast<TCH>(c - 'a' + 'A');
  }

  return c;
}

/**
 * @brief 是否是空白字符
 * @param c 字符
 * @return 如果是空白字符，返回true，否则返回false
 */
template <class TCH>
inline bool is_space(const TCH &c) {
  return ' ' == c || '\t' == c || '\r' == c || '\n' == c;
}

/**
 * @brief 移除两边或一边的空白字符
 * @param str_begin 字符串起始地址
 * @param sz 字符串长度，填0则会自动判定长度
 * @param trim_left 是否移除左边的空白字符
 * @param trim_right 是否移除右边的空白字符
 * @return 返回子串的起始地址和长度
 * @note 注意，返回的字符串是源的子串，共享地址。并且不保证以0结尾，需要用返回的长度来判定子串长度
 */
template <class TCH>
std::pair<const TCH *, size_t> trim(const TCH *str_begin, size_t sz, bool trim_left = true, bool trim_right = true) {
  if (0 == sz) {
    const TCH *str_end = str_begin;
    while (str_end && *str_end) {
      ++str_end;
    }

    sz = static_cast<size_t>(str_end - str_begin);
  }

  if (trim_left && str_begin) {
    while (*str_begin && sz > 0) {
      if (!is_space(*str_begin)) {
        break;
      }

      --sz;
      ++str_begin;
    }
  }

  size_t sub_str_sz = sz;
  if (trim_right && str_begin) {
    while (sub_str_sz > 0) {
      if (is_space(str_begin[sub_str_sz - 1])) {
        --sub_str_sz;
      } else {
        break;
      }
    }
  }

  return std::make_pair(str_begin, sub_str_sz);
}

/**
 * @brief 翻转字符串
 * @param begin 字符串起始地址
 * @param end 字符串结束地址,填入NULL，则从begin开是找到\0结束
 */
template <class TCH, class TCHE>
void reverse(TCH *begin, TCHE end_any) {
  TCH *end = reinterpret_cast<TCH *>(end_any);
  if (nullptr == begin) {
    return;
  }

  if (nullptr == end) {
    end = begin;
    while (*end) {
      ++end;
    }
  }

  if (begin >= end) {
    return;
  }

  --end;
  using std::swap;
  while (begin < end) {
    swap(*begin, *end);
    ++begin;
    --end;
  }
}

template <class TCH>
inline void reverse(TCH *begin, int end_any) {
  reverse<TCH, TCH *>(begin, reinterpret_cast<TCH *>(static_cast<intptr_t>(end_any)));
}

template <class TCH>
inline void reverse(TCH *begin, std::nullptr_t) {
  reverse<TCH, TCH *>(begin, static_cast<TCH *>(nullptr));
}

template <class T>
size_t int2str_unsigned(char *str, size_t strsz, T in) {
  if (0 == strsz) {
    return 0;
  }

  if (0 == in) {
    *str = '0';
    return 1;
  }

  size_t ret = 0;
  while (ret < strsz && in > 0) {
    str[ret] = static_cast<char>((in % 10) + '0');

    in /= 10;
    ++ret;
  }

  if (in > 0 && ret >= strsz) {
    return 0;
  }

  reverse(str, str + ret);
  return ret;
}

template <class T>
size_t int2str_signed(char *str, size_t strsz, T in) {
  if (0 == strsz) {
    return 0;
  }

  if (in < 0) {
    *str = '-';
    size_t ret = int2str_unsigned(str + 1, strsz - 1, static_cast<typename std::make_unsigned<T>::type>(-in));
    if (0 == ret) {
      return 0;
    }

    return ret + 1;
  } else {
    return int2str_unsigned(str, strsz, static_cast<typename std::make_unsigned<T>::type>(in));
  }
}

template <class T>
struct int2str_helper {
  using value_type_s = T;
  using value_type_u = typename std::make_unsigned<T>::type;

  static inline size_t call(char *str, size_t strsz, value_type_s in) { return int2str_signed(str, strsz, in); }

  static inline size_t call(char *str, size_t strsz, value_type_u in) { return int2str_unsigned(str, strsz, in); }
};

/**
 * @brief 整数转字符串
 * @param str 输出的字符串缓冲区
 * @param strsz 字符串缓冲区长度
 * @param in 输入的数字
 * @return 返回输出的数据长度，失败返回0
 */
template <class T>
inline size_t int2str(char *str, size_t strsz, const T &in) {
  size_t ret = int2str_helper<typename std::make_signed<typename std::remove_cv<T>::type>::type>::call(str, strsz, in);
  if (ret < strsz) {
    str[ret] = 0;
  }

  return ret;
}

/**
 * @brief 字符串转整数
 * @param out 输出的整数
 * @param str 被转换的字符串
 * @note 性能肯定比sscanf系，和iostream系高。strtol系就不知道了
 */
template <class T, class TCHAR>
const TCHAR *str2int(T &out, const TCHAR *str, size_t strsz = 0) {
  out = static_cast<T>(0);
  if (nullptr == str || !(*str)) {
    return str;
  }

  size_t cur = 0;

  // negative
  bool is_negative = false;
  while ((0 == strsz || cur < strsz) && (str[cur] && str[cur] == '-')) {
    is_negative = !is_negative;
    ++cur;
  }

  while ((0 == strsz || cur < strsz) && (str[cur] && is_space(str[cur]))) {
    ++cur;
  }

  if (!str[cur]) {
    return str + cur;
  }

  if ((0 == strsz || cur + 1 < strsz) && '0' == str[cur] && 'x' == tolower(str[cur + 1])) {  // hex
    for (cur += 2; (0 == strsz || cur < strsz) && str[cur]; ++cur) {
      char c = tolower(str[cur]);
      if (c >= '0' && c <= '9') {
        out = static_cast<T>(out << 4);
        out = static_cast<T>(out + static_cast<T>(c - static_cast<char>('0')));
      } else if (c >= 'a' && c <= 'f') {
        out = static_cast<T>(out << 4);
        out = static_cast<T>(out + static_cast<T>(c - static_cast<char>('a') + 10));
      } else {
        break;
      }
    }
  } else if ((0 == strsz || cur < strsz) && '\\' == str[cur]) {  // oct
    for (++cur; (0 == strsz || cur < strsz) && (str[cur] >= '0' && str[cur] < '8'); ++cur) {
      out = static_cast<T>(out << 3);
      out = static_cast<T>(out + static_cast<T>(str[cur] - static_cast<char>('0')));
    }
  } else {  // dec
    for (; (0 == strsz || cur < strsz) && (str[cur] >= '0' && str[cur] <= '9'); ++cur) {
      out = static_cast<T>(out * 10);
      out = static_cast<T>(out + static_cast<T>(str[cur] - static_cast<char>('0')));
    }
  }

  if (is_negative) {
    out = static_cast<T>((~out) + 1);
  }

  return str + cur;
}

template <class T, class TCHAR>
const TCHAR *str2int(T &out, const std::basic_string<TCHAR> &str) {
  return str2int(out, str.c_str(), str.size());
}

#ifdef __cpp_lib_string_view
template <class T, class TCHAR>
const TCHAR *str2int(T &out, std::basic_string_view<TCHAR> str) {
  return str2int(out, str.data(), str.size());
}
#endif

/**
 * @brief 字符串转整数
 * @param str 被转换的字符串
 * @return 输出的整数
 */
template <class T, class TINPUT>
inline T to_int(TINPUT &&input) {
  T ret = 0;
  str2int(ret, std::forward<TINPUT>(input));
  return ret;
}

/**
 * @brief 字符转十六进制表示
 * @param out 输出的字符串(缓冲区长度至少为2)
 * @param c 被转换的字符
 * @param upper_case 输出大写字符？
 */
template <class TStr, class TCh>
void hex(TStr *out, TCh c, bool upper_case = false) {
  out[0] = static_cast<TStr>((c >> 4) & 0x0F);
  out[1] = static_cast<TStr>(c & 0x0F);

  for (int i = 0; i < 2; ++i) {
    if (out[i] > 9) {
      TStr base;
      if (upper_case) {
        base = static_cast<TStr>('A');
      } else {
        base = static_cast<TStr>('a');
      }
      base = static_cast<TStr>(base - 10);
      out[i] = static_cast<TStr>(out[i] + base);
    } else {
      out[i] = static_cast<TStr>(out[i] + static_cast<TStr>('0'));
    }
  }
}

/**
 * @brief 字符转8进制表示
 * @param out 输出的字符串(缓冲区长度至少为3)
 * @param c 被转换的字符
 * @param upper_case 输出大写字符？
 */
template <class TStr, class TCh>
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
template <class TCh>
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
template <class Elem, class Traits>
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
template <class TCh>
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
template <class Elem, class Traits>
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
