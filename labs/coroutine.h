#pragma once

#include "labs/shell.h"
#include "util/coroutine.h"

// // state for your coroutine implementation:
struct f_t{
    // data local to coroutine between successive calls
    //insert your code here
    int i;
    int j;
    int ret;
    bool done;
    // int ans;
};

//counter typedef coroutine_t
//     // store program counter from where the execution has to carry on in another structure
//     //insert your code here


void shell_step_coroutine(shellstate_t& stateinout, coroutine_t& f_coro, f_t& f_locals);


