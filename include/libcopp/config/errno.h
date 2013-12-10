#ifndef _COPP_CONFIG_ERRNO_H_
#define _COPP_CONFIG_ERRNO_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

namespace copp
{
    enum copp_error_code
    {
        COPP_EC_SUCCESS = 0,

        COPP_EC_NOT_INITED                          = -1001,
        COPP_EC_FCONTEXT_MAKE_FAILED      = - 2001,
    };
} 

#endif
