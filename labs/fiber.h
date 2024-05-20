#pragma once

#include "labs/shell.h"
#include "util/fiber.h"

//int ret_fiber;
//bool done_fiber;

void rigourousTesting_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n);
void fact_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n);
void fib_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n);

void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize);