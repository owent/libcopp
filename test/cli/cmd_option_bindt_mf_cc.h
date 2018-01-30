#ifndef UTIL_CLI_CMDOPTIONBINDT_MF_CC_H
#define UTIL_CLI_CMDOPTIONBINDT_MF_CC_H

#pragma once

#include "libcopp/utils/config/compiler_features.h"

/*
 * cmd_option_bindt_mf_cc.h
 *
 *  Created on: 2012-01-18
 *      Author: OWenT
 *
 * 成员函数绑定器
 */

namespace util {
    namespace cli {
        // 绑定器集合
        namespace binder {
            // ============================
            // ===        类绑定        ===
            // ============================

            template <typename _T, typename _F>
            class cmd_option_bindt_mf_cc_caller {
            private:
                _F mem_func_;

            public:
                cmd_option_bindt_mf_cc_caller(_F f) : mem_func_(f) {}

#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES
                template <typename _TCBP, typename... _Args>
                void operator()(_TCBP &param, _T *arg0, _Args &... args) {
                    (arg0->*mem_func_)(param, args...);
                }

                template <typename _TCBP, typename... _Args>
                void operator()(_TCBP &param, _T &arg0, _Args &... args) {
                    (arg0.*mem_func_)(param, args...);
                }
#else

                template <typename _TCBP>
                void operator()(_TCBP &args, _T *arg0) {
                    (arg0->*mem_func_)(args);
                }

                template <typename _TCBP>
                void operator()(_TCBP &args, _T &arg0) {
                    (arg0.*mem_func_)(args);
                }

                template <typename _TCBP, typename _Arg1>
                void operator()(_TCBP &args, _T *arg0, _Arg1 &arg1) {
                    (arg0->*mem_func_)(args, arg1);
                }

                template <typename _TCBP, typename _Arg1>
                void operator()(_TCBP &args, _T &arg0, _Arg1 &arg1) {
                    (arg0.*mem_func_)(args, arg1);
                }

                template <typename _TCBP, typename _Arg1, typename _Arg2>
                void operator()(_TCBP &args, _T *arg0, _Arg1 &arg1, _Arg2 &arg2) {
                    (arg0->*mem_func_)(args, arg1, arg2);
                }

                template <typename _TCBP, typename _Arg1, typename _Arg2>
                void operator()(_TCBP &args, _T &arg0, _Arg1 &arg1, _Arg2 &arg2) {
                    (arg0.*mem_func_)(args, arg1, arg2);
                }

                template <typename _TCBP, typename _Arg1, typename _Arg2, typename _Arg3>
                void operator()(_TCBP &args, _T *arg0, _Arg1 &arg1, _Arg2 &arg2, _Arg3 &arg3) {
                    (arg0->*mem_func_)(args, arg1, arg2, arg3);
                }

                template <typename _TCBP, typename _Arg1, typename _Arg2, typename _Arg3>
                void operator()(_TCBP &args, _T &arg0, _Arg1 &arg1, _Arg2 &arg2, _Arg3 &arg3) {
                    (arg0.*mem_func_)(args, arg1, arg2, arg3);
                }
#endif
            };
        } // namespace binder
    }     // namespace cli
} // namespace util
#endif /* _CMDOPTIONBINDT_MF_CC_H_ */
