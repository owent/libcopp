#ifndef UTIL_CLI_CMDOPTIONBIND_H
#define UTIL_CLI_CMDOPTIONBIND_H

#pragma once

/*
 * cmd_option_bind.h
 *
 *  Created on: 2011-12-29
 *      Author: OWenT
 *
 * 应用程序命令处理
 * 绑定器模板（处理所有绑定的函数和类成员函数）
 */

#include "cli/cmd_option_bindt_cc.h"
#include "cli/cmd_option_bindt_mf_cc.h"

namespace util {
namespace cli {
// 标准指令处理函数(无返回值，参数为选项的映射表)
// void function_name (cmd_option_list&, [参数]); // 函数参数可选
// void function_name (callback_param, [参数]); // 函数参数可选

enum cmd_option_char {
  SPLITCHAR = 0x0001,  // (第一位)分隔符
  STRINGSYM = 0x0002,  // (第二位)字符串开闭符
  TRANSLATE = 0x0004,  // (第三位)转义字符
  CMDSPLIT = 0x0008,   // (第四位)指令分隔符
};
}  // namespace cli
}  // namespace util
#endif /* _CMDOPTIONBIND_H_ */
