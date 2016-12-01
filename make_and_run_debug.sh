#!/bin/sh

export LSAN_OPTIONS=suppressions=`pwd`/SanitizerSuppr.txt
export ASAN_OPTIONS="detect_leaks=0,allocator_may_return_null=1,suppressions=SanitizerSuppr.txt,abort_on_error=1,new_delete_type_mismatch=0,alloc_dealloc_mismatch=0" # new_delete_type_mismatch=0 because of qt5. 
export UBSAN_OPTIONS="suppressions=`pwd`/SanitizerSuppr.txt:print_stacktrace=1:halt_on_error=1"
export TSAN_OPTIONS="history_size=7,second_deadlock_stack=1,suppressions=SanitizerSuppr.txt"

make && gdb ./jack_capture

