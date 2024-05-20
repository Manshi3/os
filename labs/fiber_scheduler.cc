#include "labs/fiber_scheduler.h"

// newly added
void rigourousTesting_fiber_sed(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *n)
{
    addr_t &main_stack = *pmain_stack;
    addr_t &f_stack = *pf_stack;
    int &f_ret = *pret;
    bool &f_done = *pdone;

    int i, j;
    ////hoh_debug("13_Rtest entered");
    // 20000 iterations for 10 seconds in fiber due to context switch overhead. Normally 200000000 should be enough
    for (i = 2; i <= 10000; i++)
    {
        f_ret = 1;
        ////hoh_debug("18_Rtest entered");
        for (j = 1; j <= *n; j++) // changed to start from 1
        {
            f_ret *= j;
            f_done = false;
            ////hoh_debug("23-------\tRtest called ------");
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

// USAGE -> deep_copy(a,b[index]);
void deep_copy(char a[], char *b)
{
    int i = 0;
    for (; a[i] != '\0'; i++)
    {
        b[i] = a[i];
    }
    b[i] = '\0';
    ////hoh_debug("12_a: " << a);
    ////hoh_debug("13_b: " << b);
}

void update_state_init_fiber_scheduler(shellstate_t &stateinout)
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
    for (int i = 0; i < stateinout.bufferPos; i++)
    {
        stateinout.CommandOutput[i] = *x;
    }
    stateinout.bufferPos = 0;
    // ////hoh_debug("Called 36----------------------------");
}

void update_command_out(shellstate_t &stateinout)
{
    char *x = "";
    int off = 0;
    ////hoh_debug("WHERE-CURR Inside ");
    for (int i = 0; i < stateinout.sch_no; i++)
    {
        // ////hoh_debug("WHERE-CURR OUTPUT3: "<<off);
        for (int j = 0; j < SHELL_BUFFER_LEN && (stateinout.fibr_inputs[i][j] != NULL); j++)
            stateinout.CommandOutput[off++] = stateinout.fibr_inputs[i][j];
        stateinout.CommandOutput[off++] = ' ';
        int ans = stateinout.fibr_params[i];
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
                stateinout.CommandOutput[off++] = p[i - j - 1];
            }
        }
        stateinout.CommandOutput[off++] = ':';
        if (stateinout.got_output[i])
            for (int j = 0; j < SHELL_BUFFER_LEN && (stateinout.fibr_outputs[i][j] != NULL); j++)
                stateinout.CommandOutput[off++] = stateinout.fibr_outputs[i][j];
        stateinout.CommandOutput[off++] = '/';
    }
    if (off > 0)
        stateinout.CommandOutput[off - 1] = *x;
    for (int u = off; u < SHELL_BUFFER_LEN; u++)
    {
        stateinout.CommandOutput[u] = *x;
    }
    // Checking this
    stateinout.rendercheck = true;
    ////hoh_debug("WHERE-CURR OUTPUT4: " << stateinout.CommandOutput);
}

void update_state_fiber_scheduler(shellstate_t &stateinout)
{

    // ////hoh_debug("Called Update State -----------------------------------");
    char *x = "";
    // CommandOuput should be "rtest 12:output;---"

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

    for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        stateinout.CommandOutput[j] = *x;
    stateinout.rendercheck = true;
}

void shell_step_fiber_scheduler(shellstate_t &stateinout, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size)
{
    if (!stateinout.longComp_fiber_s || stateinout.sch_no == 0)
    {
        return;
    }
    if (stateinout.sch_mode == false)
    {
        stateinout.sch_mode = true;
        update_state_init_fiber_scheduler(stateinout);
        // update_command_out(stateinout);
        // stateinout.rendercheck = true;
    }
    bool flag_all_done;
    //

    ////hoh_debug("Entered scheduler -------------");

    size_t array_size = arrays_size / stackptrs_size;
    ////hoh_debug("131 arrays size:" << arrays_size);
    ////hoh_debug("132 array size:" << array_size);
    ////hoh_debug("132 stackptrs size:" << stackptrs_size);
    flag_all_done = true;
    for (int i = 0; i < stateinout.sch_no; i++)
    {
        ////hoh_debug("119--" << stateinout.done_fiber_array[i]);
        ////hoh_debug("120-i" << i);
        if (stateinout.done_fiber_array[i] == false)
        {
            ////hoh_debug("123-flagall_done: " << flag_all_done);
            flag_all_done = false;
        }
    }
    if (flag_all_done)
    {
        // stateinout.is_backgrounding = false;
        stateinout.longComp_fiber_s = false;

        ////hoh_debug("130-flag EXITING SCHEDULER, COMPUTATION DONE: " << flag_all_done);

        stateinout.IsEnter = true;
        stateinout.rendercheck = true;
        stateinout.sch_mode = false;
        ////hoh_debug("Setting to false");
        stateinout.sch_no = 0;
        update_state_fiber_scheduler(stateinout);
        update_command_out(stateinout);
        char *x = "";
        for (int j = 0; j < 5; j++)
            for (int i = 0; i < SHELL_BUFFER_LEN && (stateinout.fibr_inputs[j][i] != NULL); i++)
                stateinout.fibr_inputs[j][i] = NULL;

        for (int j = 0; j < 5; j++)
            for (int i = 0; i < SHELL_BUFFER_LEN && (stateinout.fibr_outputs[j][i] != NULL); i++)
                stateinout.fibr_outputs[j][i] = NULL;
        return;
    }
    ////hoh_debug("145-flag SCH_MOD IS ON: " << flag_all_done);
    // [true, true, false, false, true]
    ////hoh_debug("136 fiber before:" << stateinout.fiber_number);
    ////hoh_debug("137: done array val" << stateinout.done_fiber_array[stateinout.fiber_number]);
    while (stateinout.done_fiber_array[stateinout.fiber_number] == true)
    {
        stateinout.fiber_number = (stateinout.fiber_number + 1) % stateinout.sch_no;
    }

    ////hoh_debug("147:::Current fiber number" << stateinout.fiber_number);
    shell_step_fiber_util(stateinout.fiber_number, stateinout, stackptrs[9],
                          stackptrs[stateinout.fiber_number], addr_t(arrays + (stateinout.fiber_number * array_size)), array_size);
    stateinout.fiber_number = (stateinout.fiber_number + 1) % stateinout.sch_no;

    // shell_step_fiber_util(stateinout.fiber_number, stateinout, stackptrs[9], \
    // stackptrs[stateinout.fiber_number], arrays, arrays_size);
    // stateinout.fiber_number = (stateinout.fiber_number+1)%stateinout.sch_no;

    //
}

int get_length(char *a)
{
    int i = 0;
    while (a[i] != '\0')
        i++;
    return i;
}

bool check_equal(char *a, char *b)
{
    if (get_length(a) != get_length(b))
        return false;
    for (int i = 0; i < get_length(a); i += 1)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

void shell_step_fiber_util(int fiber_number, shellstate_t &stateinout,
                           addr_t &main_stack, addr_t &f_stack, addr_t f_array, uint32_t f_arraysize)
{
    int n = stateinout.fibr_params[fiber_number];
    if (!stateinout.backgrounding_sch[fiber_number])
    {
        stateinout.backgrounding_sch[fiber_number] = true;

        if (check_equal("rtest", stateinout.fibr_inputs[fiber_number]))
        // if (stateinout.fibr_inputs[fiber_number] == "rtest" )
        {
            ////hoh_debug("rtest enter");
            stack_init5(f_stack, f_array, f_arraysize, &rigourousTesting_fiber_sed,
                        &main_stack, &f_stack, &stateinout.ret_fiber_array[fiber_number],
                        &stateinout.done_fiber_array[fiber_number], &stateinout.fibr_params[fiber_number]);
        }
        // debug Error in Fibonacci <- acting like factorial
        else if (check_equal("fib", stateinout.fibr_inputs[fiber_number]))
        // else if (stateinout.fibr_inputs[fiber_number] == "fib")
        {
            ////hoh_debug("fib enter");
            stack_init5(f_stack, f_array, f_arraysize, &fib_fiber,
                        &main_stack, &f_stack, &stateinout.ret_fiber_array[fiber_number],
                        &stateinout.done_fiber_array[fiber_number], &stateinout.fibr_params[fiber_number]);
        }
        else
        {
            ////hoh_debug("fact enter");
            stack_init5(f_stack, f_array, f_arraysize, &fact_fiber,
                        &main_stack, &f_stack, &stateinout.ret_fiber_array[fiber_number],
                        &stateinout.done_fiber_array[fiber_number], &stateinout.fibr_params[fiber_number]);
        }
        ////hoh_debug("calling 1st time stack savestore--------");
        ////hoh_debug("226_done before val: " << stateinout.done_fiber_array[fiber_number]);
        stack_saverestore(main_stack, f_stack);
        ////hoh_debug("226_fiber_val: " << stateinout.fiber_number);
        ////hoh_debug("226_done val: " << stateinout.done_fiber_array[fiber_number]);
        ////hoh_debug("ans ret 227--" << stateinout.ret_fiber_array[fiber_number]);
    }
    else
    {
        if (!stateinout.done_fiber_array[fiber_number])
        {
            // ////hoh_debug("fiber_number: "<< fiber_number);
            // ////hoh_debug("191_About to Context switch_s---------------");
            // stateinout.got_output[fiber_number] = false;
            stack_saverestore(main_stack, f_stack);

            ////hoh_debug("235_fiber_val: " << stateinout.fiber_number);
            ////hoh_debug("235_done val: " << stateinout.done_fiber_array[fiber_number]);
            ////hoh_debug("ans ret 237--" << stateinout.ret_fiber_array[fiber_number]);

            if (stateinout.done_fiber_array[fiber_number])
            {
                ////hoh_debug("ans ret 273--" << stateinout.ret_fiber_array[fiber_number]);
                int ans = stateinout.ret_fiber_array[fiber_number];
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
                // for(int j=0; j<stateinout.renderingStringSize; j++)
                // {
                //     stateinout.fibr_outputs[fiber_number][j] = stateinout.renderingString[j];
                // }
                ////hoh_debug("Before deep copy: rs, fo 297--" << stateinout.renderingString << stateinout.fibr_outputs[fiber_number]);
                ////hoh_debug("Before deep copy: fiber outputs 298--" << stateinout.fibr_outputs[0] << stateinout.fibr_outputs[1] << stateinout.fibr_outputs[2] << stateinout.fibr_outputs[3] << stateinout.fibr_outputs[4]);

                deep_copy(stateinout.renderingString, stateinout.fibr_outputs[fiber_number]);

                ////hoh_debug("After deep copy: rs, fo 301--" << stateinout.renderingString << stateinout.fibr_outputs[fiber_number]);
                ////hoh_debug("After deep copy: fiber outputs 302--" << stateinout.fibr_outputs[0] << stateinout.fibr_outputs[1] << stateinout.fibr_outputs[2] << stateinout.fibr_outputs[3] << stateinout.fibr_outputs[4]);
                // zinda hai abhinava?

                // stateinout.fibr_outputs[fiber_number] = stateinout.renderingString;

                // for(int j=stateinout.renderingStringSize; j<get_length(stateinout.fibr_outputs[fiber_number]); j++)
                // {
                //     stateinout.fibr_outputs[fiber_number][j]=stateinout.renderingString[j];
                // }

                stateinout.got_output[fiber_number] = true;

                ////hoh_debug("227-size: " << sizeof(stateinout.fibr_outputs[fiber_number]));
                ////hoh_debug("255-retval: " << stateinout.fibr_outputs[fiber_number]);
                ////hoh_debug("256-fiber: " << fiber_number);

                /// for fiber number 0
                ////hoh_debug("306-output of 0 in fibr: " << stateinout.fibr_outputs[0]);

                stateinout.backgrounding_sch[fiber_number] = false;
                update_command_out(stateinout);
                ////hoh_debug("HERE-CURR OUTPUT1: " << stateinout.fibr_outputs[fiber_number]);
                ////hoh_debug("WHERE-CURR OUTPUT2: " << stateinout.CommandOutput);
            }
        }
    }
}

// lol

// void shell_step_fiber_util(int fiber_number, shellstate_t &stateinout, addr_t &main_stack, addr_t &f_stack, addr_t f_array, uint32_t f_arraysize)
// {
// // insert your code here
// ////hoh_debug("enter shell step fiber util");
// if (!stateinout.longComp_fiber_s)
// return;
// if (!stateinout.is_backgrounding)
// {
// int n = stateinout.fibr_params[fiber_number];
// // f_locals->i = 1;
// stateinout.done_fiber_array[fiber_number] = false;
// stateinout.is_backgrounding = true;

// ///////////
// stateinout.IsEnter = false;

// ///////////
// // if (stateinout.buffer[0] == 'r')
// // {
// // stack_init5(f_stack, &f_array, f_arraysize, &rigourousTesting_fiber, &main_stack, &f_stack, &stateinout.ret_fiber, &stateinout.done_fiber, &stateinout.fiber_params[0]);
// // }
// // else if (stateinout.buffer[0] == 'f')
// // {
// if (stateinout.fibr_inputs[fiber_number] == "rtest")
// {
// ////hoh_debug("rtest enter");
// stack_init5(f_stack, &f_array, f_arraysize, &rigourousTesting_fiber,
// &main_stack, &f_stack, &stateinout.ret_fiber_array[fiber_number],
// &stateinout.done_fiber_array[fiber_number], &stateinout.fibr_params[fiber_number]);
// }
// else if (stateinout.fibr_inputs[fiber_number] == "fib")
// {
// ////hoh_debug("fib enter");
// stack_init5(f_stack, &f_array, f_arraysize, &fib_fiber,
// &main_stack, &f_stack, &stateinout.ret_fiber_array[fiber_number],
// &stateinout.done_fiber_array[fiber_number], &stateinout.fibr_params[fiber_number]);
// }
// else
// {
// ////hoh_debug("fact enter");
// stack_init5(f_stack, &f_array, f_arraysize, &fact_fiber,
// &main_stack, &f_stack, &stateinout.ret_fiber_array[fiber_number],
// &stateinout.done_fiber_array[fiber_number], &stateinout.fibr_params[fiber_number]);
// }

// // ////hoh_debug("calling rigorou test fiber -----------------------------------");
// stack_saverestore(main_stack, f_stack);
// // ////hoh_debug("After init 117");
// }
// else
// {
// if (!stateinout.done_fiber_array[fiber_number])
// {
// ////hoh_debug("fiber_number: "<< fiber_number);
// ////hoh_debug("191_About to Context switch_s---------------");
// stack_saverestore(main_stack, f_stack);
// ////hoh_debug("193_finished Context switch_s---------------");
// }
// else
// {
// ////hoh_debug("ans ret 227--"<<stateinout.ret_fiber_array[fiber_number]);
// int ans = stateinout.ret_fiber_array[fiber_number];
// char p[100];
// if (ans == 0)
// {
// p[0] = '0';
// p[1] = '\0';
// }
// else
// {
// int i = 0;
// while (ans != 0)
// {
// p[i++] = ans % 10 + '0';
// ans /= 10;
// }
// p[i] = '\0';
// for (int j = 0; j < i; j++)
// {
// stateinout.renderingStringSize += 1;
// stateinout.renderingString[j] = p[i - j - 1];
// }

// // stateinout.fibr_outputs[fiber_number] = stateinout.renderingString;

// for(int j=0;j<stateinout.renderingStringSize;j++)
// {
// stateinout.fibr_outputs[fiber_number][j]=stateinout.renderingString[j];
// }
// stateinout.got_output[fiber_number] = true;
// ////hoh_debug("255-retval"<<stateinout.fibr_outputs[fiber_number]);
// ////hoh_debug("256-fiber"<<fiber_number);
// // for(int j=0;j<1;j++)
// // {
// // stateinout.fibr_outputs[fiber_number][j]=stateinout.renderingString[j];
// // }

// // stateinout.fibr_outputs[fiber_number] = "hello";

// }

// // stack_init5(f_stack, &f_array, f_arraysize, &rigourousTesting_fiber, &main_stack, &f_stack, &stateinout.ret_fiber, &stateinout.done_fiber, &stateinout.fiber_params[0]);
// // stateinout.is_backgrounding = false;
// // stateinout.longComp_fiber = false;

// //////
// stateinout.IsEnter = true;
// //////
// stateinout.rendercheck = true;
// update_state_fiber_scheduler(stateinout);
// }
// }
// }
