#ifndef UTIL_CLI_CMDOPTIONBINDT_CC_H
#define UTIL_CLI_CMDOPTIONBINDT_CC_H

#pragma once

/*
 * cmd_option_bindt_cc.h
 *
 *  Created on: 2012-01-18
 *      Author: OWenT
 *
 * bing to normal function
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

  template <typename _TCBP, typename... _Args>
  void operator()(_TCBP &param, _Args &...args) {
    func_(param, args...);
  }
};
}  // namespace binder
}  // namespace cli
}  // namespace util
#endif /* cmd_option_bindt_cc_caller */
