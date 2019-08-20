#!/bin/bash

cd "$(dirname "$0")";

ALL_FILES=($(ls asm/*.asm asm/*.S)) ;

perl -p -i -e 's;\bjump_fcontext\b;copp_jump_fcontext;g' ${ALL_FILES[@]};
perl -p -i -e 's;\b_jump_fcontext\b;_copp_jump_fcontext;g' ${ALL_FILES[@]};
perl -p -i -e 's;\b_make_fcontext\b;_copp_make_fcontext;g' ${ALL_FILES[@]};
perl -p -i -e 's;\bmake_fcontext\b;copp_make_fcontext;g' ${ALL_FILES[@]};
perl -p -i -e 's;\b_ontop_fcontext\b;_copp_ontop_fcontext;g' ${ALL_FILES[@]};
perl -p -i -e 's;\bontop_fcontext\b;copp_ontop_fcontext;g' ${ALL_FILES[@]};

perl -p -i -e 's;\bBOOST_CONTEXT_EXPORT\b;EXPORT;g' ${ALL_FILES[@]};
perl -p -i -e 's;\bBOOST_USE_TSX\b;LIBCOPP_FCONTEXT_USE_TSX;g' ${ALL_FILES[@]};
