#include "labs/fiber.h"

void rigourousTesting_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n)
{
    addr_t &main_stack = *pmain_stack;
    addr_t &f_stack = *pf_stack;
    int &f_ret = *pret;
    bool &f_done = *pdone;

    int i, j;
    ////hoh_debug("11_Rtest entered");
    // 20000 iterations for 10 seconds in fiber due to context switch overhead. Normally 200000000 should be enough
    for (i = 2; i <= 20000; i++)
    {
        f_ret = 1;
        ////hoh_debug("16_Rtest entered");
        for (j = 2; j <= *n; j++)
        {
            f_ret *= j;
            f_done = false;
            ////hoh_debug("19-------\tRtest called ------");
            stack_saverestore(f_stack, main_stack);
        }
    }
    // ////hoh_debug("21_Outside_for_loop");
    for (;;)
    {
        f_done = true;
        stack_saverestore(f_stack, main_stack);
    }
}

void fib_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n)
{
    addr_t &main_stack = *pmain_stack;
    addr_t &f_stack = *pf_stack;
    int &f_ret = *pret;
    bool &f_done = *pdone;

    if (*n == 0)
    {
        f_ret = 0;
        for (;;)
        {
            f_done = true;
            stack_saverestore(f_stack, main_stack);
        }
        return;
    }
    if (*n == 1)
    {
        f_ret = 1;
        for (;;)
        {
            f_done = true;
            stack_saverestore(f_stack, main_stack);
        }
        return;
    }

    int x = 0;
    int y = 1;

    for (int i = 2; i <= *n; i++)
    {
        int temp = x;
        x = y;
        y += temp;
        f_done = false;
        stack_saverestore(f_stack, main_stack);
    }
    f_ret = y;
    for (;;)
    {
        f_done = true;
        stack_saverestore(f_stack, main_stack);
    }
    return;
}

void fact_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n)
{
    addr_t &main_stack = *pmain_stack;
    addr_t &f_stack = *pf_stack;
    int &f_ret = *pret;
    bool &f_done = *pdone;

    f_ret = 1;

    for (int i = 2; i <= *n; i++)
    {
        f_ret *= i;
        f_done = false;
        stack_saverestore(f_stack, main_stack);
    }
    for (;;)
    {
        f_done = true;
        stack_saverestore(f_stack, main_stack);
    }
    return;
}

// void fib_recur(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n)
// {
//     addr_t &main_stack = *pmain_stack;
//     addr_t &f_stack = *pf_stack;
//     int &f_ret = *pret;
//     bool &f_done = *pdone;
//     //int i, j;
//     if (*n <= 1)
//     {
//         f_ret = *n;
//         f_done = true;
//         stack_saverestore(f_stack, main_stack);
//     }
//     else
//     {
//         fib_recur(pmain_stack, pf_stack, &var1, &pdone1, n - 2);
//         fib_recur(pmain_stack, pf_stack, &var2, &pdone2, n - 1);
//         //int var1, var2;
//         bool pdone1 = false, pdone2 = false;
//         //stack_init5(pf_stack, &f_array, f_arraysize, &fib_recur, &main_stack, &f_stack, &stateinout.ret_fiber, &stateinout.done_fiber, *n-1);
//         /*
//         if (!pdone1)fib_recur(pmain_stack, pf_stack, &var1, &pdone1, n - 2);
//         if (!pdone2)fib_recur(pmain_stack, pf_stack, &var2, &pdone2, n - 1);
//         */
//         //fib_recur(pmain_stack, pf_stack, &var1, &pdone1, n - 2);
//         //fib_recur(pmain_stack, pf_stack, &var2, &pdone2, n - 1);

//         ////hoh_debug("var1: " << var1);
//         ////hoh_debug("var2: " << var2);
//         f_ret += var1 + var2;
//         f_done = false;
//         stack_saverestore(f_stack, main_stack);
//     }
//     for (;;)
//     {
//         f_done = true;
//         stack_saverestore(f_stack, main_stack);
//     }
// }

void update_state_init_fiber(shellstate_t &stateinout)
{
    char *x = "";
    for (int i = 0; i < stateinout.bufferPos; i++)
    {
        stateinout.ShellCommand[i] = stateinout.PreviousCommand[i] = stateinout.buffer[i];
    }
    for (int i = 0; i < stateinout.bufferPos; i++)
    {
        stateinout.buffer[i] = *x;
    }
    stateinout.bufferPos = 0;
    // ////hoh_debug("Called 36----------------------------");
}

void update_state_fiber(shellstate_t &stateinout)
{

    ////hoh_debug("Called Update State -----------------------------------");
    char *x = "";
    for (int i = 0; i < stateinout.renderingStringSize; i++)
    {
        stateinout.CommandOutput[i] = stateinout.renderingString[i];
    }

    for (int i = 0; i < stateinout.bufferPos; i++)
    {
        stateinout.ShellCommand[i] = stateinout.buffer[i];
    }

    for (int i = stateinout.bufferPos; i < SHELL_BUFFER_LEN; i++)
    {
        stateinout.ShellCommand[i] = *x;
    }

    for (int i = 0; i < stateinout.renderingStringSize; i++)
    {
        stateinout.renderingString[i] = *x;
    }

    stateinout.renderingStringSize = 0;

    /////////

    ////hoh_debug("Update State, Buffer: " << stateinout.buffer);
    ////hoh_debug("Update State, BufferPos: " << stateinout.bufferPos);

    /////////////////////////

    // stateinout.bufferPos = 0;

    stateinout.renderingStringSize = 0;
    stateinout.totalcommands++;
    int i;
    if (stateinout.totalcommands > 6)
    {
        for (i = 1; i < 6; i++)
        {
            for (int j = 0; j < SHELL_BUFFER_LEN; j++) // hello?
                stateinout.last_commands[i - 1][j] = stateinout.last_commands[i][j];
            for (int j = 0; j < SHELL_BUFFER_LEN; j++)
                stateinout.last_outputs[i - 1][j] = stateinout.last_outputs[i][j];
        }
        stateinout.totalcommands = 6;
    }

    for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        stateinout.last_commands[stateinout.totalcommands - 1][j] = stateinout.PreviousCommand[j];

    for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        stateinout.PreviousCommand[j] = *x;

    for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        stateinout.last_outputs[stateinout.totalcommands - 1][j] = stateinout.CommandOutput[j];
}

void shell_step_fiber(shellstate_t &stateinout, addr_t &main_stack, addr_t &f_stack, addr_t f_array, uint32_t f_arraysize)
{
    // insert your code here
    if (!stateinout.longComp_fiber)
        return;
    if (!stateinout.is_backgrounding)
    {
        int n = stateinout.fiber_params[0];
        // f_locals->i = 1;
        stateinout.done_fiber = false;
        stateinout.is_backgrounding = true;

        ///////////
        stateinout.IsEnter = false;

        ///////////
        update_state_init_fiber(stateinout);
        // if (stateinout.buffer[0] == 'r')
        // {
        // stack_init5(f_stack, &f_array, f_arraysize, &rigourousTesting_fiber, &main_stack, &f_stack, &stateinout.ret_fiber, &stateinout.done_fiber, &stateinout.fiber_params[0]);
        // }
        // else if (stateinout.buffer[0] == 'f')
        // {
        stack_init5(f_stack, &f_array, f_arraysize, &rigourousTesting_fiber, &main_stack, &f_stack, &stateinout.ret_fiber, &stateinout.done_fiber, &stateinout.fiber_params[0]);
        // }

        //    ////hoh_debug("calling rigorou test fiber -----------------------------------");
        stack_saverestore(main_stack, f_stack);
        //    ////hoh_debug("After init 117");
    }
    else
    {
        if (!stateinout.done_fiber)
        {
            //  ////hoh_debug("125_About to Context switch");
            stack_saverestore(main_stack, f_stack);
            //  ////hoh_debug("119_Outside");
        }
        else
        {
            int ans = stateinout.ret_fiber;
            char p[100];
            if (ans == 0)
            {
                p[0] = '0';
                p[1] = '\0';
            }
            else
            {
                int i = 0;
                while (ans != 0)
                {
                    p[i++] = ans % 10 + '0';
                    ans /= 10;
                }
                p[i] = '\0';
                for (int j = 0; j < i; j++)
                {
                    stateinout.renderingStringSize += 1;
                    stateinout.renderingString[j] = p[i - j - 1];
                }
            }

            // stack_init5(f_stack, &f_array, f_arraysize, &rigourousTesting_fiber, &main_stack, &f_stack, &stateinout.ret_fiber, &stateinout.done_fiber, &stateinout.fiber_params[0]);
            stateinout.is_backgrounding = false;
            stateinout.longComp_fiber = false;

            //////
            stateinout.IsEnter = true;
            //////
            stateinout.rendercheck = true;
            update_state_fiber(stateinout);
        }
    }
}
