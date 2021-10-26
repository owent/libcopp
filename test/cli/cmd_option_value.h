#ifndef UTIL_CLI_CMDOPTIONVALUE_H
#define UTIL_CLI_CMDOPTIONVALUE_H

#pragma once

/*
 * cmd_option_value.h
 *
 *  Created on: 2011-12-29
 *      Author: OWenT
 *
 * 应用程序命令处理
 *
 */

#include <stdint.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <common/string_oprs.h>

namespace util {
namespace cli {

template <typename Tt>
struct string2any;

template <>
struct string2any<std::string> {
  inline std::string operator()(const std::string &s) const { return s; }
};

template <>
struct string2any<char> {
  inline char operator()(const std::string &s) const { return s.empty() ? 0 : s[0]; }
};

template <>
struct string2any<unsigned char> {
  inline unsigned char operator()(const std::string &s) const {
    return static_cast<unsigned char>(s.empty() ? 0 : s[0]);
  }
};

template <>
struct string2any<int16_t> {
  inline int16_t operator()(const std::string &s) const { return util::string::to_int<int16_t>(s.c_str()); }
};

template <>
struct string2any<uint16_t> {
  inline uint16_t operator()(const std::string &s) const { return util::string::to_int<uint16_t>(s.c_str()); }
};

template <>
struct string2any<int32_t> {
  inline int32_t operator()(const std::string &s) const { return util::string::to_int<int32_t>(s.c_str()); }
};

template <>
struct string2any<uint32_t> {
  inline uint32_t operator()(const std::string &s) const { return util::string::to_int<uint32_t>(s.c_str()); }
};

template <>
struct string2any<int64_t> {
  inline int64_t operator()(const std::string &s) const { return util::string::to_int<int64_t>(s.c_str()); }
};

template <>
struct string2any<uint64_t> {
  inline uint64_t operator()(const std::string &s) const { return util::string::to_int<uint64_t>(s.c_str()); }
};

template <>
struct string2any<bool> {
  inline bool operator()(const std::string &s) const { return !s.empty() && "0" != s; }
};

template <typename Tt>
struct string2any {
  inline Tt operator()(const std::string &s) const {
    Tt ret;
    std::stringstream ss;
    ss.str(s);
    ss >> ret;
    return ret;
  }
};

class cmd_option_value {
 protected:
  std::string data_;

 public:
  cmd_option_value(const char *str_data);
  cmd_option_value(const char *begin, const char *end);
  cmd_option_value(const std::string &str_data);

  template <typename Tr>
  Tr to() const {
    typedef typename ::std::remove_cv<Tr>::type cv_type;
    return string2any<cv_type>()(data_);
  }

  // 获取存储对象的字符串
  const std::string &to_cpp_string() const;

  bool to_bool() const;

  char to_char() const;

  short to_short() const;

  int to_int() const;

  long to_long() const;

  long long to_longlong() const;

  double to_double() const;

  float to_float() const;

  const char *to_string() const;

  unsigned char to_uchar() const;

  unsigned short to_ushort() const;

  unsigned int to_uint() const;

  unsigned long to_ulong() const;

  unsigned long long to_ulonglong() const;

  int8_t to_int8() const;

  uint8_t to_uint8() const;

  int16_t to_int16() const;

  uint16_t to_uint16() const;

  int32_t to_int32() const;

  uint32_t to_uint32() const;

  int64_t to_int64() const;

  uint64_t to_uint64() const;

  // ============ logic operation ============
  bool to_logic_bool() const;

  void split(char delim, std::vector<cmd_option_value> &out);
};
}  // namespace cli
}  // namespace util

#endif /* _CMDOPTIONVALUE_H_ */
