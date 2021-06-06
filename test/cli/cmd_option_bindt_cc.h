#ifndef UTIL_CLI_CMDOPTIONBINDT_CC_H
#define UTIL_CLI_CMDOPTIONBINDT_CC_H

#pragma once

#include "libcopp/utils/config/compiler_features.h"

/*
 * cmd_option_bindt_cc.h
 *
 *  Created on: 2012-01-18
 *      Author: OWenT
 *
 * 自由函数绑定器
 */

namespace util {
namespace cli {
// 绑定器集合
namespace binder {
// ============================
// ===       函数绑定       ===
// ============================
template <typename _TF>
class cmd_option_bindt_cc_caller {
 private:
  _TF func_;

 public:
  cmd_option_bindt_cc_caller(_TF f) : func_(f) {}

#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES
  template <typename _TCBP, typename... _Args>
  void operator()(_TCBP &param, _Args &...args) {
    func_(param, args...);
  }

#else
  template <typename _TCBP>
  void operator()(_TCBP &args) {
    func_(args);
  }

  template <typename _TCBP, typename _Arg0>
  void operator()(_TCBP &args, _Arg0 &arg0) {
    func_(args, arg0);
  }

  template <typename _TCBP, typename _Arg0, typename _Arg1>
  void operator()(_TCBP &args, _Arg0 &arg0, _Arg1 &arg1) {
    func_(args, arg0, arg1);
  }

  template <typename _TCBP, typename _Arg0, typename _Arg1, typename _Arg2>
  void operator()(_TCBP &args, _Arg0 &arg0, _Arg1 &arg1, _Arg2 &arg2) {
    func_(args, arg0, arg1, arg2);
  }
#endif
};
}  // namespace binder
}  // namespace cli
}  // namespace util
#endif /* cmd_option_bindt_cc_caller */
