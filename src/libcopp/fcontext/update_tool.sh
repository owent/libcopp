#!/bin/sh

cd "$(dirname "$PWD")";

perl -p -i -e 's;\bjump_fcontext\b;copp_jump_fcontext;g' asm/*.asm asm/*.S;
perl -p -i -e 's;\b_jump_fcontext\b;_copp_jump_fcontext;g' asm/*.asm asm/*.S;
perl -p -i -e 's;\b_make_fcontext\b;_copp_make_fcontext;g' asm/*.asm asm/*.S;
perl -p -i -e 's;\bmake_fcontext\b;copp_make_fcontext;g' asm/*.asm asm/*.S;
