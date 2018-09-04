#ifndef UTIL_CLI_CMDOPTIONBINDPARAMLIST_H
#define UTIL_CLI_CMDOPTIONBINDPARAMLIST_H

#pragma once

/*
 * cmd_option_bind_param_list.h
 *
 *  Created on: 2014-01-22
 *      Author: OWenT
 *
 * 绑定器参数列表类
 */

#include "cli/cmd_option_bindt_cc.h"
#include "cli/cmd_option_bindt_mf_cc.h"
#include "cli/cmd_option_list.h"

#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES
#include "std/tuple.h"
#endif

namespace util {
    namespace cli {
        namespace binder {

            /**
             *  Maps member pointers into instances of _Mem_fn but leaves all
             *  primary template handles the non--member-pointer case.
             */
            template <typename _Tp>
            struct maybe_wrap_member_pointer {
                typedef _Tp type;
                typedef cmd_option_bindt_cc_caller<_Tp> caller_type;

                static const _Tp &do_wrap(const _Tp &__x) { return __x; }

                static _Tp &do_wrap(_Tp &__x) { return __x; }
            };


            /**
             *  Maps member pointers into instances of _Mem_fn but leaves all
             *  partial specialization handles the member pointer case.
             */
            template <typename _Tp, typename _Class>
            struct maybe_wrap_member_pointer<_Tp _Class::*> {
                typedef _Tp _Class::*type;
                typedef cmd_option_bindt_mf_cc_caller<_Class, type> caller_type;

                static type do_wrap(_Tp _Class::*__pm) { return type(__pm); }
            };

// ============================
// ===       参数列表       ===
// ============================
#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES
            template <typename... _Args>
            class cmd_option_bind_param_list {
            private:
                /**
                 * 用于创建存储对象索引和解包索引[0, 1, 2, ..., sizeof...(_Args) - 1]
                 */
                template <int... _Index>
                struct index_args_var_list {};

                template <std::size_t N, int... _Index>
                struct build_args_index : build_args_index<N - 1, _Index..., sizeof...(_Index)> {};

                template <int... _Index>
                struct build_args_index<0, _Index...> {
                    typedef index_args_var_list<_Index...> type;
                };

            private:
                template <class _F, int... _Indexes>
                void _do_call(_F &f, callback_param args, index_args_var_list<_Indexes...>) {
                    f(args, std::get<_Indexes>(args_)...);
                }

            private:
                std::tuple<_Args...> args_;

            public:
                cmd_option_bind_param_list(_Args... args) : args_(args...) {}

                template <class _F>
                void operator()(_F &f, callback_param args, int) {
                    typedef typename build_args_index<sizeof...(_Args)>::type _index_type;
                    _do_call(f, args, _index_type());
                }
            };

#else

            class cmd_option_bind_param_list0 {
            public:
                template <class _F>
                void operator()(_F &f, callback_param args, int) {
                    f(args);
                }
            };

            template <typename _Arg0>
            class cmd_option_bind_param_list1 {
            private:
                _Arg0 arg0_;

            public:
                cmd_option_bind_param_list1(_Arg0 arg0) : arg0_(arg0) {}

                template <class _F>
                void operator()(_F &f, callback_param args, int) {
                    f(args, arg0_);
                }
            };

            template <typename _Arg0, typename _Arg1>
            class cmd_option_bind_param_list2 {
            private:
                _Arg0 arg0_;
                _Arg1 arg1_;

            public:
                cmd_option_bind_param_list2(_Arg0 arg0, _Arg1 arg1) : arg0_(arg0), arg1_(arg1) {}

                template <class _F>
                void operator()(_F &f, callback_param args, int) {
                    f(args, arg0_, arg1_);
                }
            };

            template <typename _Arg0, typename _Arg1, typename _Arg2>
            class cmd_option_bind_param_list3 {
            private:
                _Arg0 arg0_;
                _Arg1 arg1_;
                _Arg2 arg2_;

            public:
                cmd_option_bind_param_list3(_Arg0 arg0, _Arg1 arg1, _Arg2 arg2) : arg0_(arg0), arg1_(arg1), arg2_(arg2) {}

                template <class _F>
                void operator()(_F &f, callback_param args, int) {
                    f(args, arg0_, arg1_, arg2_);
                }
            };

            template <typename _Arg0, typename _Arg1, typename _Arg2, typename _Arg3>
            class cmd_option_bind_param_list4 {
            private:
                _Arg0 arg0_;
                _Arg1 arg1_;
                _Arg2 arg2_;
                _Arg2 arg3_;

            public:
                cmd_option_bind_param_list4(_Arg0 arg0, _Arg1 arg1, _Arg2 arg2, _Arg2 arg3)
                    : arg0_(arg0), arg1_(arg1), arg2_(arg2), arg3_(arg3) {}

                template <class _F>
                void operator()(_F &f, callback_param args, int) {
                    f(args, arg0_, arg1_, arg2_, arg3_);
                }
            };

#endif
        } // namespace binder
    }     // namespace cli
} // namespace util
#endif /* _CMDOPTIONBINDPARAMLIST_H_ */
