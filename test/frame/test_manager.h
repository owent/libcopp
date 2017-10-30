/*
 * test_manager.h
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef TEST_MANAGER_H_
#define TEST_MANAGER_H_

#pragma once

#include <ctime>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include "shell_font.h"
#include "std/type_traits/is_xxx_impl.h"

#include "test_case_base.h"

/*
 *
 */
class test_manager {
public:
    typedef test_case_base *case_ptr_type;
    typedef std::vector<std::pair<std::string, case_ptr_type> > test_type;
    typedef std::map<std::string, test_type> test_data_type;

public:
    test_manager();
    virtual ~test_manager();

    void append(const std::string &test_name, const std::string &case_name, case_ptr_type);

    int run();

    static test_manager &me();

    int *success_counter_ptr;
    int *failed_counter_ptr;

    static std::string get_expire_time(clock_t begin, clock_t end);

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST
    static boost::unit_test::test_suite *&test_suit();
#endif

    template <typename TL, typename TR, bool has_pointer, bool has_integer, bool all_integer>
    struct pick_param;

    // compare pointer with integer
    template <typename TL, typename TR>
    struct pick_param<TL, TR, true, true, false> {
        template <typename T>
        intptr_t operator()(const T &t) {
            return (intptr_t)(t);
        }
    };

    // compare integer with integer, all converted to int64_t
    template <typename TL, typename TR>
    struct pick_param<TL, TR, false, true, true> {
        template <typename T>
        int64_t operator()(const T &t) {
            return static_cast<int64_t>(t);
        }
    };

    template <typename TL, typename TR, bool has_pointer, bool has_integer, bool all_integer>
    struct pick_param {
        template <typename T>
        const T &operator()(const T &t) {
            return t;
        }
    };

    // expect functions
    template <typename TL, typename TR>
    bool expect_eq(const TL &l, const TR &r, const char *lexpr, const char *rexpr, const char *file, size_t line) {
        pick_param<TL, TR, util::type_traits::is_pointer<TL>::value || util::type_traits::is_pointer<TR>::value,
                   util::type_traits::is_integral<TL>::value || util::type_traits::is_integral<TR>::value,
                   util::type_traits::is_integral<TL>::value && util::type_traits::is_integral<TR>::value>
            pp;
        if (pp(l) == pp(r)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected: " << lexpr << " == " << rexpr << std::endl
                 << lexpr << ": " << l << std::endl
                 << rexpr << ": " << r << std::endl;

            return false;
        }
    }

    template <typename TL, typename TR>
    bool expect_ne(const TL &l, const TR &r, const char *lexpr, const char *rexpr, const char *file, size_t line) {
        pick_param<TL, TR, util::type_traits::is_pointer<TL>::value || util::type_traits::is_pointer<TR>::value,
                   util::type_traits::is_integral<TL>::value || util::type_traits::is_integral<TR>::value,
                   util::type_traits::is_integral<TL>::value && util::type_traits::is_integral<TR>::value>
            pp;

        if (pp(l) != pp(r)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected: " << lexpr << " ！= " << rexpr << std::endl
                 << lexpr << ": " << l << std::endl
                 << rexpr << ": " << r << std::endl;

            return false;
        }
    }

    template <typename TL, typename TR>
    bool expect_lt(const TL &l, const TR &r, const char *lexpr, const char *rexpr, const char *file, size_t line) {
        pick_param<TL, TR, util::type_traits::is_pointer<TL>::value || util::type_traits::is_pointer<TR>::value,
                   util::type_traits::is_integral<TL>::value || util::type_traits::is_integral<TR>::value,
                   util::type_traits::is_integral<TL>::value && util::type_traits::is_integral<TR>::value>
            pp;

        if (pp(l) < pp(r)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected: " << lexpr << " < " << rexpr << std::endl
                 << lexpr << ": " << l << std::endl
                 << rexpr << ": " << r << std::endl;

            return false;
        }
    }

    template <typename TL, typename TR>
    bool expect_le(const TL &l, const TR &r, const char *lexpr, const char *rexpr, const char *file, size_t line) {
        pick_param<TL, TR, util::type_traits::is_pointer<TL>::value || util::type_traits::is_pointer<TR>::value,
                   util::type_traits::is_integral<TL>::value || util::type_traits::is_integral<TR>::value,
                   util::type_traits::is_integral<TL>::value && util::type_traits::is_integral<TR>::value>
            pp;

        if (pp(l) <= pp(r)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected: " << lexpr << " <= " << rexpr << std::endl
                 << lexpr << ": " << l << std::endl
                 << rexpr << ": " << r << std::endl;

            return false;
        }
    }

    template <typename TL, typename TR>
    bool expect_gt(const TL &l, const TR &r, const char *lexpr, const char *rexpr, const char *file, size_t line) {
        pick_param<TL, TR, util::type_traits::is_pointer<TL>::value || util::type_traits::is_pointer<TR>::value,
                   util::type_traits::is_integral<TL>::value || util::type_traits::is_integral<TR>::value,
                   util::type_traits::is_integral<TL>::value && util::type_traits::is_integral<TR>::value>
            pp;

        if (pp(l) > pp(r)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected: " << lexpr << " > " << rexpr << std::endl
                 << lexpr << ": " << l << std::endl
                 << rexpr << ": " << r << std::endl;

            return false;
        }
    }

    template <typename TL, typename TR>
    bool expect_ge(const TL &l, const TR &r, const char *lexpr, const char *rexpr, const char *file, size_t line) {
        pick_param<TL, TR, util::type_traits::is_pointer<TL>::value || util::type_traits::is_pointer<TR>::value,
                   util::type_traits::is_integral<TL>::value || util::type_traits::is_integral<TR>::value,
                   util::type_traits::is_integral<TL>::value && util::type_traits::is_integral<TR>::value>
            pp;

        if (pp(l) >= pp(r)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected: " << lexpr << " >= " << rexpr << std::endl
                 << lexpr << ": " << l << std::endl
                 << rexpr << ": " << r << std::endl;

            return false;
        }
    }

    template <typename TL>
    bool expect_true(const TL &l, const char *expr, const char *file, size_t line) {
        if (!!(l)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected true: " << expr << std::endl
                 << expr << ": " << l << std::endl;

            return false;
        }
    }

    template <typename TL>
    bool expect_false(const TL &l, const char *expr, const char *file, size_t line) {
        if (!(l)) {
            ++(*success_counter_ptr);
            return true;
        } else {
            ++(*failed_counter_ptr);
            util::cli::shell_stream ss(std::cout);
            ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "FAILED => " << file << ":" << line << std::endl
                 << "Expected false: " << expr << std::endl
                 << expr << ": " << l << std::endl;

            return false;
        }
    }

private:
    test_data_type tests_;
    int success_;
    int failed_;
};

int run_tests();

#endif /* TEST_MANAGER_H_ */
