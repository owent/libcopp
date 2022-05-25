#!/bin/bash

# --max-stackframe=137417071008  -> -DLIBCOPP_ENABLE_VALGRIND=ON -DVALGRIND_ROOT=...
valgrind --vgdb=yes --leak-check=full --tool=memcheck --show-leak-kinds=all \
  --max-threads=2000 --malloc-fill=0x5e --free-fill=0x5f \
  --log-file="$1.memcheck.log" "$@"
