#!/bin/bash

env ASAN_OPTIONS=halt_on_error=1:abort_on_error=1:detect_leaks=1:disable_coredump=0:handle_segv=1:handle_abort=0:handle_sigill=0:handle_sigfpe=1:unmap_shadow_on_exit=1:atexit=1:allow_addr2line=1:log_path=$PWD/asan.log \
    "$@"