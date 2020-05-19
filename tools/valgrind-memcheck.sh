#!/bin/bash

valgrind --vgdb=yes --leak-check=full --tool=memcheck --show-leak-kinds=all \
    --max-stackframe=137417071008 --malloc-fill=0x5e --free-fill=0x5f       \
    --log-file=memcheck.log "$@"