#ifndef UTIL_CLI_CMDOPTIONBINDTBASE_H
#define UTIL_CLI_CMDOPTIONBINDTBASE_H

#pragma once

/*
 * cmd_option_bindt_base.h
 *
 *  Created on: 2012-01-18
 *      Author: OWenT
 *
 * 绑定器基类及公共定义
 */

#include "cli/cmd_option_bind_param_list.h"
#include "cli/cmd_option_list.h"

namespace util {
namespace cli {
// 标准指令处理函数参数类型

// 标准指令处理函数(无返回值，参数为选项的映射表)
// void function_name (cmd_option_list&, [参数]); // 函数参数可选
// void function_name (callback_param, [参数]); // 函数参数可选

// 绑定器集合
namespace binder {
// 函数、函数结构绑定器
template <class _F, class _PL>
class cmd_option_bindt : public cmd_option_bind_base {
 protected:
  _F func_obj_;     // 函数结构
  _PL param_list_;  // 参数列表结构

 public:
  typedef cmd_option_bindt this_type;

  cmd_option_bindt(_F f, _PL l) : func_obj_(f), param_list_(l) {}

  void operator()(callback_param args) { param_list_(func_obj_, args, 0); }
};
}  // namespace binder
}  // namespace cli
}  // namespace util
#endif /* _CMDOPTIONBINDTBASE_H_ */
