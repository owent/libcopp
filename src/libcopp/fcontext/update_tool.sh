#!/bin/bash

cd "$(dirname "$0")"

ALL_FILES=($(ls asm/*.asm asm/*.S))

perl -p -i -e 's;\bjump_fcontext\b;copp_jump_fcontext_v2;g' ${ALL_FILES[@]}
perl -p -i -e 's;\b_jump_fcontext\b;_copp_jump_fcontext_v2;g' ${ALL_FILES[@]}
perl -p -i -e 's;\b_make_fcontext\b;_copp_make_fcontext_v2;g' ${ALL_FILES[@]}
perl -p -i -e 's;\bmake_fcontext\b;copp_make_fcontext_v2;g' ${ALL_FILES[@]}
perl -p -i -e 's;\b_ontop_fcontext\b;_copp_ontop_fcontext_v2;g' ${ALL_FILES[@]}
perl -p -i -e 's;\bontop_fcontext\b;copp_ontop_fcontext_v2;g' ${ALL_FILES[@]}

perl -p -i -e 's;\bBOOST_CONTEXT_EXPORT\b;EXPORT;g' ${ALL_FILES[@]}
perl -p -i -e 's;\bBOOST_USE_TSX\b;LIBCOPP_FCONTEXT_USE_TSX;g' ${ALL_FILES[@]}
perl -p -i -e 's;\bBOOST_CONTEXT_TLS_STACK_PROTECTOR\b;LIBCOPP_FCONTEXT_TLS_STACK_PROTECTOR;g' ${ALL_FILES[@]}
perl -p -i -e 's;\bBOOST_CONTEXT_SHADOW_STACK\b;LIBCOPP_FCONTEXT_SHADOW_STACK;g' ${ALL_FILES[@]}

for CPP_FILE in asm/*.cpp; do
  perl -p -i -e 's;\bboost/context/detail/fcontext.hpp\b;libcopp/fcontext/fcontext.hpp;g' "$CPP_FILE"
  perl -p -i -e 's;\bboost::context::detail::\b;copp::;g' "$CPP_FILE"
done
