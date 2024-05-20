#include "labs/coroutine.h"

// CUSTOM
int rigourousTesting_async(int n)
{
    int ans = 1;
    for (int i = 2; i <= 200000000; i++)
    {
        hoh_debug("hi lwde " << i);
        int tmp = 1;
        for (int ii = 2; ii <= n; ii++)
        {
            tmp *= ii;
        }
        if (i % 100000 == 0)
            hoh_debug("i: " << i);
        ans = tmp;
    }
    hoh_debug("hi ans " << ans);
    return ans;
}

// isBackgrounding = True
// maintain buffer
//
void update_state(shellstate_t &stateinout);
void update_state_init(shellstate_t &stateinout);
void rigourousTesting_async_yes(coroutine_t *pf_coro, f_t *pf_locals, int n)
{
    coroutine_t &f_coro = *pf_coro; // boilerplate: to ease the transition from existing code

    // hoh_debug("--------------------------------------------");
    // hoh_debug("i:" << pf_locals->i);
    // hoh_debug("j:" << pf_locals->j);
    // hoh_debug("done:" << pf_locals->done);
    // hoh_debug("ret:" << pf_locals->ret);
    // //hoh_debug("done:" << *pdone);
    // //hoh_debug("ret:" << *pret);
    // //hoh_debug("params:" << pf_locals->params[0]);
    // hoh_debug("--------------------------------------------");

    // int &ans = pf_locals->ans;

    int &i = pf_locals->i;
    int &j = pf_locals->j;
    int &ret = pf_locals->ret;
    bool &done = pf_locals->done;

    //int& ret = *pret;
    //bool& done = *pdone;

    // ret = 1;

    h_begin(f_coro);

    for (i = 2; i <= /*20000*/ 20000; i++)
    {
        ret = 1;
        // hoh_debug("coroutine, j: " << j);
        // hoh_debug("coroutine, n: " << n);

        for (j = 2; j <= n; j++)
        {
            ret *= j;
            done = false;

            // hoh_debug("H_yield: " << i << " " << j << " " << ret);
            h_yield(f_coro);
        }
        // hoh_debug("coroutine, j: " << j);

        // pf_locals->j=2;
        //if (i % 10 == 0)hoh_debug("i: " << i);
    }

    //   hoh_debug("hi ans " << ret);

    done = true;
    h_end(f_coro);
}

void shell_step_coroutine(shellstate_t &stateinout, coroutine_t &f_coro, f_t &f_locals)
{

    if (!stateinout.longComp_coroutine)
        return;

    // hoh_debug("-----------------------------------");
    // hoh_debug("i:" << f_locals.i);
    // hoh_debug("j:" << f_locals.j);
    // hoh_debug("done:" << f_locals.done);
    // hoh_debug("ret:" << f_locals.ret);
    // hoh_debug("backgrounding:" << stateinout.is_backgrounding);
    // hoh_debug("-----------------------------------");

    // first time calling:
    // 1. parse the input
    // 2. call the coroutine
    if (!stateinout.is_backgrounding)
    {
        int n = stateinout.coroutine_params[0];
        // f_locals->i = 1;
        f_locals.done = false;
        stateinout.is_backgrounding = true;

        ///////////
        stateinout.IsEnter = false;
        ///////////
        update_state_init(stateinout);
        rigourousTesting_async_yes(&f_coro, &f_locals, stateinout.coroutine_params[0]);
    }

    // next calling
    // 1. check if coroutine is done
    // 2. if not, call the coroutine
    // reset
    else
    {
        if (!f_locals.done)
        {
            // nth function call where n>1
            rigourousTesting_async_yes(&f_coro, &f_locals, stateinout.coroutine_params[0]);
        }
        else
        {
            // After the last call of the coroutine. Function has ended here
            int ans = f_locals.ret;
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
            coroutine_reset(f_coro);
            stateinout.is_backgrounding = false;
            stateinout.longComp_coroutine = false;

            //////
            stateinout.IsEnter = true;
            //////

            hoh_debug("147_Coroutine_rendercheck");
            stateinout.rendercheck = true;
            update_state(stateinout);
        }
    }
}

void update_state_init(shellstate_t &stateinout)
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
    hoh_debug("Called C 163----------------------------");
}

void update_state(shellstate_t &stateinout)
{

    hoh_debug("Called Update State -----------------------------------");
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

    hoh_debug("Update State, Buffer: " << stateinout.buffer);
    hoh_debug("Update State, BufferPos: " << stateinout.bufferPos);

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

// void shell_step_coroutine_util(shellstate_t &stateinout, coroutine_t &f_coro, f_t &f_locals)
// {
//    // stateinout.longComp_coroutine;
//     if(!stateinout.IsEnter)return;

//     if(stateinout.is_backgrounding)
//     {
//       if(!stateinout.f_locals.done)
//       {
//         // define variables in shellstate_t
//         rigourousTesting_async_yes(&stateinout.f_coro, &stateinout.f_locals, stateinout.f_locals.params[0]);
//       }
//       else
//       {
//         stateinout.is_backgrounding = false;
//         update_state(stateinout);
//         stateinout.IsEnter = false;
//         // rendering
//       }
//       return;
//     }

//     hoh_debug("shell_step_coroutine: Enter Pressed");
//     hoh_debug("Shell_Step_Coroutine: Buffer:" << stateinout.buffer);
//     hoh_debug("Shell_Step_Coroutine: BufferSize:" << stateinout.bufferPos);

//     if (stateinout.bufferPos >= 8 && stateinout.buffer[0] == 'r' && stateinout.buffer[1] == 't' && stateinout.buffer[2] == 'e' && stateinout.buffer[3] == 's' && stateinout.buffer[4] == 't' && stateinout.buffer[5] == ' ')
//     {
//         bool valid = true;
//         uint32_t val = 0;

//         // Look for space between fact i.e. function and argument
//         for (int i = 6; i < stateinout.bufferPos - 2; i++)
//         {
//             if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
//             {
//                 valid = false;
//                 break;
//             }
//             val = val * 10 + (stateinout.buffer[i] - '0');
//         }
//         hoh_debug("VAL: " << val);
//         if (!valid)
//         {
//             hoh_debug("Invalid input");
//             // stateinout.renderingString = "Invalid input";
//         }
//         else
//         {
//             if (val > 10)
//                 val = 10;

//             // store these in shellstate_t
//               coroutine_t f_coro;
//               coroutine_reset(f_coro);
//               f_t f_locals;
//             //

//             hoh_debug("Val (before coroutine): " << val);

//             if (!stateinout.is_backgrounding)
//             {
//                 f_locals.i = 2;
//                 f_locals.j = 2;
//                 f_locals.done = false;
//                 stateinout.f_locals.params[0] = val;
//             }

//             hoh_debug("--------------------------------------------------------------------------------------------------");

//             stateinout.is_backgrounding = true;
//             // rigourousTesting_async_yes(&stateinout.f_coro, &stateinout.f_locals, stateinout.f_locals.params[0], 1, false);
//             // else stateinout.is_backgrounding = false;

//             if(stateinout.is_backgrounding)return;

//             int ans = f_locals.ret;
//             hoh_debug("ANS: " << ans);

//             char p[100];
//             if (ans == 0)
//             {
//                 p[0] = '0';
//                 p[1] = '\0';
//             }
//             else
//             {
//                 int i = 0;
//                 while (ans != 0)
//                 {
//                     p[i++] = ans % 10 + '0';
//                     ans /= 10;
//                 }
//                 p[i] = '\0';
//                 for (int j = 0; j < i; j++)
//                 {
//                     stateinout.renderingStringSize += 1;
//                     stateinout.renderingString[j] = p[i - j - 1];
//                 }
//             }
//         }
//     }

//     // execute these instructions on backgrounding=False and complete=True
//     // if(stateinout.is_backgrounding)return;

// }

// void shell_step_coroutine(shellstate_t& stateinout, coroutine_t& f_coro, f_t& f_locals)
// {
//   //
//     if (stateinout.bufferPos >= 6 && stateinout.buffer[0] == 'r' && stateinout.buffer[1] == 't' && stateinout.buffer[2] == 'e' && stateinout.buffer[3] == 's' && stateinout.buffer[4] == 't' && stateinout.buffer[5] == ' '){
//         bool valid = true;
//       uint32_t val = 0;

//       // Look for space between fact i.e. function and argument
//       for (int i = 6; i < stateinout.bufferPos; i++)
//       {
//         if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
//         {
//           valid = false;
//           break;
//         }
//         val = val * 10 + (stateinout.buffer[i] - '0');
//       }
//       hoh_debug("VAL: " << val);
//       if (!valid)
//       {
//         hoh_debug("Invalid input");
//         // stateinout.renderingString = "Invalid input";
//       }
//       else{
//         if (val > 10)
//             val = 10;
//         coroutine_reset(f_coro);
//         if(!done){
//             rigourousTesting_async_yes(f_coro, f_locals, val, 1, false);
//         }
//       }
//     }
// }