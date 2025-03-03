// Copyright 2025 owent
// Created by owent on 2017-05-18

#pragma once

#include <libcopp/utils/memory/intrusive_ptr.h>

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace util {

template <typename T>
using intrusive_ptr = LIBCOPP_COPP_NAMESPACE_ID::memory::intrusive_ptr<T>;

using LIBCOPP_COPP_NAMESPACE_ID::memory::const_pointer_cast;
using LIBCOPP_COPP_NAMESPACE_ID::memory::dynamic_pointer_cast;
using LIBCOPP_COPP_NAMESPACE_ID::memory::get_pointer;
using LIBCOPP_COPP_NAMESPACE_ID::memory::static_pointer_cast;
using LIBCOPP_COPP_NAMESPACE_ID::memory::swap;

}  // namespace util
LIBCOPP_COPP_NAMESPACE_END
