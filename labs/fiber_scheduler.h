#pragma once


#include "labs/shell.h"
#include "util/fiber.h"

//////
#include "labs/fiber.h"
//////

void shell_step_fiber_util(int fiber_number, shellstate_t &stateinout, addr_t &main_stack, addr_t &f_stack, addr_t f_array, uint32_t f_arraysize);

void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size);


