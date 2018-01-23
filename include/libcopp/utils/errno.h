#ifndef COPP_UTILS_ERRNO_H
#define COPP_UTILS_ERRNO_H


#pragma once

namespace copp {
    /**
     * error code
     */
    enum copp_error_code {
        COPP_EC_SUCCESS = 0, //!< COPP_EC_SUCCESS

        COPP_EC_UNKNOWN = -101,                //!< COPP_EC_UNKNOWN
        COPP_EC_EXTERNAL_INSERT_FAILED = -102, //!< COPP_EC_EXTERNAL_INSERT_FAILED
        COPP_EC_EXTERNAL_ERASE_FAILED = -103,  //!< COPP_EC_EXTERNAL_ERASE_FAILED
        COPP_EC_IN_RESET = -104,               //!< COPP_EC_IN_RESET

        COPP_EC_ALLOC_STACK_FAILED = -201, //!< COPP_EC_ALLOC_STACK_FAILED

        COPP_EC_NOT_INITED = -1001,       //!< COPP_EC_NOT_INITED
        COPP_EC_ALREADY_INITED = -1002,   //!< COPP_EC_ALREADY_INITED
        COPP_EC_ACCESS_VIOLATION = -1003, //!< COPP_EC_ACCESS_VIOLATION
        COPP_EC_NOT_READY = -1004,        //!< COPP_EC_NOT_READY
        COPP_EC_NOT_RUNNING = -1005,      //!< COPP_EC_NOT_RUNNING
        COPP_EC_IS_RUNNING = -1006,       //!< COPP_EC_IS_RUNNING
        COPP_EC_ALREADY_FINISHED = -1007, //!< COPP_EC_ALREADY_FINISHED
        COPP_EC_NOT_FOUND = -1008,        //!< COPP_EC_NOT_FOUND
        COPP_EC_ALREADY_EXIST = -1009,    //!< COPP_EC_ALREADY_EXIST
        COPP_EC_ARGS_ERROR = -1010,       //!< COPP_EC_ARGS_ERROR
        COPP_EC_CAST_FAILED = -1011,      //!< COPP_EC_CAST_FAILED

        COPP_EC_FCONTEXT_MAKE_FAILED = -2001, //!< COPP_EC_FCONTEXT_MAKE_FAILED
    };
} // namespace copp

#endif
