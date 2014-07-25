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

#include "shell_font.h"

#define SHELL_FONT_SET_OPT_END "\033[24;25;27;49;0m"

shell_font::shell_font(int iFlag): m_iFlag(iFlag)
{
}

shell_font::~shell_font()
{
}

std::string shell_font::GetStyleCode(int iFlag)
{
    std::string ret;
    ret.reserve(32);
    ret = "\033[";
    bool bFirst = true;

    // 第一部分，特殊样式
    if (iFlag & SHELL_FONT_SPEC_BOLD)
    {
        ret += std::string((!bFirst)?";":"") + "1";
        bFirst = false;
    }
    if (iFlag & SHELL_FONT_SPEC_UNDERLINE)
    {
        ret += std::string((!bFirst)?";":"") + "4";
        bFirst = false;
    }
    if (iFlag & SHELL_FONT_SPEC_FLASH)
    {
        ret += std::string((!bFirst)?";":"") + "5";
        bFirst = false;
    }
    if (iFlag & SHELL_FONT_SPEC_DARK)
    {
        ret += std::string((!bFirst)?";":"") + "2";
        bFirst = false;
    }

    // 前景色
    iFlag >>= 8;
    if (iFlag & 0xff)
    {
        std::string base = "30";
        int iStart = 0;
        for (; iStart < 8 && !(iFlag & (1 << iStart)); ++ iStart);
        if (iStart < 8)
            base[1] += iStart;
        ret += std::string((!bFirst)?";":"") + base;
        bFirst = false;
    }

    // 背景色
    iFlag >>= 8;
    if (iFlag & 0xff)
    {
        std::string base = "40";
        int iStart = 0;
        for (; iStart < 8 && !(iFlag & (1 << iStart)); ++ iStart);
        if (iStart < 8)
            base[1] += iStart;
        ret += std::string((!bFirst)?";":"") + base;
        // bFirst = false; no need to set because not used later
    }

    ret += "m";

    return ret;
}

std::string shell_font::GetStyleCode()
{
    return GetStyleCode(m_iFlag);
}

std::string shell_font::GetStyleCloseCode()
{
    return SHELL_FONT_SET_OPT_END;
}

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
    char* term_name = getenv("TERM");
    if (NULL != term_name)
        my_term_name = term_name;
    std::transform(my_term_name.begin(), my_term_name.end(), my_term_name.begin(), ::tolower);

    if (color_term.end() == color_term.find(my_term_name))
        return -1;
    return 1;
}

std::string shell_font::GenerateString(const std::string& strInput, int iFlag)
{
    static int status_ = 0;

    if (0 == status_) {
        status_ = _check_term_color_status();
    }

    if (status_ < 0 || iFlag == 0)
        return strInput;
    return GetStyleCode(iFlag) + strInput + GetStyleCloseCode();
}

std::string shell_font::GenerateString(const std::string& strInput)
{
    return GenerateString(strInput, m_iFlag);
}
