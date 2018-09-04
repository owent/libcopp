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

#include <cstring>
#include <sstream>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <vector>


#include "libcopp/utils/std/smart_ptr.h"

#include <common/string_oprs.h>

namespace util {
    namespace cli {
        class cmd_option_value {
        protected:
            std::string data_;

            struct string2any {

                template <typename Tt>
                static inline Tt conv(const std::string &s, Tt *) {
                    Tt ret;
                    std::stringstream ss;
                    ss.str(s);
                    ss >> ret;
                    return ret;
                }

                static inline const std::string &conv(const std::string &s, std::string *) { return s; }

                // static const char *conv(const std::string &s, const char **) { return s.c_str(); }

                static inline char conv(const std::string &s, char *) { return s.empty() ? 0 : s[0]; }

                static inline unsigned char conv(const std::string &s, unsigned char *) {
                    return static_cast<unsigned char>(s.empty() ? 0 : s[0]);
                }

                static inline int16_t conv(const std::string &s, int16_t *) { return util::string::to_int<int16_t>(s.c_str()); }

                static inline uint16_t conv(const std::string &s, uint16_t *) { return util::string::to_int<int16_t>(s.c_str()); }

                static inline int32_t conv(const std::string &s, int32_t *) { return util::string::to_int<int32_t>(s.c_str()); }

                static inline uint32_t conv(const std::string &s, uint32_t *) { return util::string::to_int<uint32_t>(s.c_str()); }

                static inline int64_t conv(const std::string &s, int64_t *) { return util::string::to_int<int64_t>(s.c_str()); }
                static inline uint64_t conv(const std::string &s, uint64_t *) { return util::string::to_int<uint64_t>(s.c_str()); }

                static inline bool conv(const std::string &s, bool *) { return !s.empty() && "0" != s; }
            };

        public:
            cmd_option_value(const char *str_data);
            cmd_option_value(const char *begin, const char *end);
            cmd_option_value(const std::string &str_data);

            template <typename Tr>
            Tr to() const {
                typedef typename ::std::remove_cv<Tr>::type cv_type;
                return string2any::conv(data_, reinterpret_cast<cv_type *>(NULL));
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
    } // namespace cli
} // namespace util

#endif /* _CMDOPTIONVALUE_H_ */
