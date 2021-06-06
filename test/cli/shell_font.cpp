/*
 * shell_fonts.h
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <algorithm>
#include <cstdlib>
#include <set>

#include "cli/shell_font.h"

#define SHELL_FONT_SET_OPT_END "\033[0m"

namespace util {
namespace cli {

namespace detail {
static char tolower(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 'a';
  }

  return c;
}
}  // namespace detail

shell_font::shell_font(int iFlag) : m_iFlag(iFlag) {}

shell_font::~shell_font() {}

std::string shell_font::GetStyleCode(int iFlag) {
  std::string ret;
  ret.reserve(32);
  ret = "\033[";
  bool bFirst = true;

  // 第一部分，特殊样式
  if (iFlag & shell_font_style::SHELL_FONT_SPEC_BOLD) {
    ret += "1";
    bFirst = false;
  }
  if (iFlag & shell_font_style::SHELL_FONT_SPEC_UNDERLINE) {
    ret += std::string((!bFirst) ? ";" : "") + "4";
    bFirst = false;
  }
  if (iFlag & shell_font_style::SHELL_FONT_SPEC_FLASH) {
    ret += std::string((!bFirst) ? ";" : "") + "5";
    bFirst = false;
  }
  if (iFlag & shell_font_style::SHELL_FONT_SPEC_DARK) {
    ret += std::string((!bFirst) ? ";" : "") + "2";
    bFirst = false;
  }

  // 前景色
  iFlag >>= 8;
  if (iFlag & 0xff) {
    std::string base = "30";
    int iStart = 0;
    for (; iStart < 8 && !(iFlag & (1 << iStart)); ++iStart)
      ;
    if (iStart < 8) base[1] += static_cast<char>(iStart);
    ret += std::string((!bFirst) ? ";" : "") + base;
    bFirst = false;
  }

  // 背景色
  iFlag >>= 8;
  if (iFlag & 0xff) {
    std::string base = "40";
    int iStart = 0;
    for (; iStart < 8 && !(iFlag & (1 << iStart)); ++iStart)
      ;
    if (iStart < 8) base[1] += static_cast<char>(iStart);
    ret += std::string((!bFirst) ? ";" : "") + base;
    // bFirst = false; no need to set because not used later
  }

  ret += "m";

  return ret;
}

std::string shell_font::GetStyleCode() { return GetStyleCode(m_iFlag); }

std::string shell_font::GetStyleCloseCode() { return SHELL_FONT_SET_OPT_END; }

static int _check_term_color_status() {
  std::set<std::string> color_term;
  color_term.insert("eterm");
  color_term.insert("ansi");
  color_term.insert("color-xterm");
  color_term.insert("con132x25");
  color_term.insert("con132x30");
  color_term.insert("con132x43");
  color_term.insert("con132x60");
  color_term.insert("con80x25");
  color_term.insert("con80x28");
  color_term.insert("con80x30");
  color_term.insert("con80x43");
  color_term.insert("con80x50");
  color_term.insert("con80x60");
  color_term.insert("cons25");
  color_term.insert("console");
  color_term.insert("cygwin");
  color_term.insert("dtterm");
  color_term.insert("eterm-color");
  color_term.insert("gnome");
  color_term.insert("gnome-256color");
  color_term.insert("jfbterm");
  color_term.insert("konsole");
  color_term.insert("kterm");
  color_term.insert("linux");
  color_term.insert("linux-c");
  color_term.insert("mach-color");
  color_term.insert("mlterm");
  color_term.insert("putty");
  color_term.insert("rxvt");
  color_term.insert("rxvt-256color");
  color_term.insert("rxvt-cygwin");
  color_term.insert("rxvt-cygwin-native");
  color_term.insert("rxvt-unicode");
  color_term.insert("rxvt-unicode256");
  color_term.insert("screen");
  color_term.insert("screen-256color");
  color_term.insert("screen-256color-bce");
  color_term.insert("screen-bce");
  color_term.insert("screen-256color-bce");
  color_term.insert("screen-bce");
  color_term.insert("screen-w");
  color_term.insert("screen.linux");
  color_term.insert("vt100");
  color_term.insert("xterm");
  color_term.insert("xterm-16color");
  color_term.insert("xterm-256color");
  color_term.insert("xterm-88color");
  color_term.insert("xterm-color");
  color_term.insert("xterm-debian");

  std::string my_term_name;

#ifdef _MSC_VER
  char *term_name = NULL;
  size_t term_name_len = 0;
  _dupenv_s(&term_name, &term_name_len, "TERM");
#else
  char *term_name = getenv("TERM");
#endif
  if (NULL != term_name) {
#ifdef _MSC_VER
    my_term_name.assign(term_name, term_name_len);
    ::free(term_name);
#else
    my_term_name = term_name;
#endif
  }

  std::transform(my_term_name.begin(), my_term_name.end(), my_term_name.begin(), detail::tolower);

  if (color_term.end() == color_term.find(my_term_name)) return -1;
  return 1;
}

std::string shell_font::GenerateString(const std::string &strInput, int iFlag) {
  static int status_ = 0;

  if (0 == status_) {
    status_ = _check_term_color_status();
  }

  if (status_ < 0 || iFlag == 0) return strInput;
  return GetStyleCode(iFlag) + strInput + GetStyleCloseCode();
}

std::string shell_font::GenerateString(const std::string &strInput) { return GenerateString(strInput, m_iFlag); }

#ifdef SHELL_FONT_USING_WIN32_CONSOLE

static std::map<int, WORD> &_get_flag_mapping() {
  static std::map<int, WORD> ret;
  if (ret.empty()) {
    ret[shell_font_style::SHELL_FONT_SPEC_NULL] = 0;
    ret[shell_font_style::SHELL_FONT_SPEC_BOLD] = COMMON_LVB_LEADING_BYTE;
    ret[shell_font_style::SHELL_FONT_SPEC_UNDERLINE] = COMMON_LVB_UNDERSCORE;
    ret[shell_font_style::SHELL_FONT_SPEC_FLASH] = 0;  // 不支持
    ret[shell_font_style::SHELL_FONT_SPEC_DARK] = 0;   // 不支持

    ret[shell_font_style::SHELL_FONT_COLOR_BLACK] = 0;
    ret[shell_font_style::SHELL_FONT_COLOR_RED] = FOREGROUND_RED | FOREGROUND_INTENSITY;
    ret[shell_font_style::SHELL_FONT_COLOR_GREEN] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    ret[shell_font_style::SHELL_FONT_COLOR_YELLOW] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    ret[shell_font_style::SHELL_FONT_COLOR_BLUE] = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    ret[shell_font_style::SHELL_FONT_COLOR_MAGENTA] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    ret[shell_font_style::SHELL_FONT_COLOR_CYAN] = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    ret[shell_font_style::SHELL_FONT_COLOR_WHITE] =
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_BLACK] = 0;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_RED] = BACKGROUND_RED;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_GREEN] = BACKGROUND_GREEN;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_YELLOW] = BACKGROUND_RED | BACKGROUND_GREEN;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_BLUE] = BACKGROUND_BLUE;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_MAGENTA] = BACKGROUND_RED | BACKGROUND_BLUE;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_CYAN] = BACKGROUND_BLUE | BACKGROUND_GREEN;
    ret[shell_font_style::SHELL_FONT_BACKGROUND_COLOR_WHITE] = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
  }

  return ret;
}

static WORD _get_default_color() { return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; }

#endif

shell_stream::shell_stream_opr::shell_stream_opr(stream_t *os) : pOs(os), flag(shell_font_style::SHELL_FONT_SPEC_NULL) {
#ifdef SHELL_FONT_USING_WIN32_CONSOLE
  if (os == &std::cout) {
    hOsHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  } else if (os == &std::cerr) {
    hOsHandle = GetStdHandle(STD_ERROR_HANDLE);
  } else {
    hOsHandle = NULL;
  }
#endif
}

shell_stream::shell_stream_opr::~shell_stream_opr() {
  if (NULL == pOs) {
    return;
  }

  reset();
}

shell_stream::shell_stream_opr::shell_stream_opr(const shell_stream_opr &other) { (*this) = other; }

shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::operator=(const shell_stream::shell_stream_opr &other) {
  pOs = other.pOs;

#ifdef SHELL_FONT_USING_WIN32_CONSOLE
  hOsHandle = other.hOsHandle;
#endif
  flag = shell_font_style::SHELL_FONT_SPEC_NULL;

  return (*this);
}

#if defined(UTIL_CONFIG_COMPILER_CXX_NULLPTR) && UTIL_CONFIG_COMPILER_CXX_NULLPTR
const shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::operator<<(std::nullptr_t) const {
  close();
  (*pOs) << "nullptr";
  return (*this);
}
#endif

const shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::operator<<(
    shell_font_style::shell_font_spec style) const {
  open(style);
  return (*this);
}

const shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::operator<<(
    shell_font_style::shell_font_color style) const {
  open(style);
  return (*this);
}

const shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::operator<<(
    shell_font_style::shell_font_background_color style) const {
  open(style);
  return (*this);
}

const shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::operator<<(stream_t &(*fn)(stream_t &)) const {
  close();
  (*pOs) << fn;
  return (*this);
}

const shell_stream::shell_stream_opr &shell_stream::shell_stream_opr::open(int f) const {
  if (f == shell_font_style::SHELL_FONT_SPEC_NULL) {
    reset();
    return (*this);
  }

  flag |= f;
  return (*this);
}

void shell_stream::shell_stream_opr::close() const {
  if (NULL == pOs) {
    return;
  }

  if (shell_font_style::SHELL_FONT_SPEC_NULL == flag) {
    return;
  }

#ifdef SHELL_FONT_USING_WIN32_CONSOLE
  if (NULL != hOsHandle) {
    std::map<int, WORD> &color_map = _get_flag_mapping();
    WORD style = 0;
    int left_flag = flag;

    while (left_flag) {
      int f = left_flag & (left_flag ^ (left_flag - 1));
      std::map<int, WORD>::iterator iter = color_map.find(f);
      if (iter != color_map.end()) {
        style |= iter->second;
      }

      left_flag = left_flag & (left_flag - 1);
    }

    SetConsoleTextAttribute(hOsHandle, style);
  }
#else
  (*pOs) << shell_font::GetStyleCode(flag);
#endif

  flag = shell_font_style::SHELL_FONT_SPEC_NULL;
}

void shell_stream::shell_stream_opr::reset() const {
  if (NULL == pOs) {
    return;
  }

  close();

#ifdef SHELL_FONT_USING_WIN32_CONSOLE
  if (NULL != hOsHandle) {
    SetConsoleTextAttribute(hOsHandle, _get_default_color());
  }
#else

  (*pOs) << shell_font::GetStyleCloseCode();

#endif
}

shell_stream::shell_stream(stream_t &stream) : m_pOs(&stream) {}

shell_stream::shell_stream_opr shell_stream::operator()() const { return shell_stream_opr(m_pOs); }

}  // namespace cli
}  // namespace util