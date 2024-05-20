#include "labs/shell.h"
#include "labs/vgatext.h"

void shell_init(shellstate_t &state)
{
    state.bufferPos = 0;
    state.CommandOutputSize = 0;
    // key m not working
    char *keys[] = {
        "esc",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "0",
        "-",
        "=",
        "back",
        "tab",
        "q",
        "w",
        "e",
        "r",
        "t",
        "y",
        "u",
        "i",
        "o",
        "p",
        "[",
        "]",
        "entr",
        "ctrl",
        "a",
        "s",
        "d",
        "f",
        "g",
        "h",
        "j",
        "k",
        "l",
        ";",
        "'",
        " ",
        "shft",
        "up",
        "down",
        "left",
        "right",
        "z",
        "x",
        "c",
        "v",
        "b",
        "n",
        "m",
    };
    char *scancode[] = {
        "01",
        "02",
        "03",
        "04",
        "05",
        "06",
        "07",
        "08",
        "09",
        "0a",
        "0b",
        "0c",
        "0d",
        "0e",
        "0f",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15",
        "16",
        "17",
        "18",
        "19",
        "1a",
        "1b",
        "1c",
        "1d",
        "1e",
        "1f",
        "20",
        "21",
        "22",
        "23",
        "24",
        "25",
        "26",
        "27",
        "28",
        "39",
        "2a",
        "48",
        "50",
        "4b",
        "4d",
        "2c",
        "2d",
        "2e",
        "2f",
        "30",
        "31",
        "32",
    };

    for (int i = 0; i < 53; i++)
    {
        state.keys[i] = keys[i];
        state.scancode[i] = scancode[i];
    }
    state.cursorLine = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            state.out_buffer[i][j] = ' ';
    }
    for (int i = 0; i < 5; i++)
    {
        char *x = "";
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        {
            state.fibr_outputs[i][j] = *x;
            state.fibr_inputs[i][j] = *x;
        }
    }
}

char *shellstate_t::scancode_to_key(uint8_t scankey)
{
    char val[2];
    val[1] = hex2char(scankey);
    val[0] = hex2char(scankey / 16);
    for (int i = 0; i < 53; i++)
    {
        if (val[0] == scancode[i][0] && val[1] == scancode[i][1])
        {
            return keys[i];
        }
    }

    return "";
}
void shell_update(uint8_t scankey, shellstate_t &stateinout)
{
    // hoh_debug("257_Inside_shell_update");
    // hoh_debug("Got: " << unsigned(scankey));
    stateinout.increment_key_presses();
    // hoh_debug("Key Presses: " << unsigned(stateinout.numKeysPressed));
    //  add the scankey to buffer
    stateinout.rendercheck = false;
    // stateinout.rendercheck = true;
    char *key = stateinout.scancode_to_key(scankey);
    int val = 0;
    // hoh_debug("key: " << key);
    if (key == "back" && stateinout.bufferPos > 0)
    {
        // hoh_debug("backspace");
        char *x = "";
        stateinout.buffer[stateinout.bufferPos - 1] = *x;
        stateinout.bufferPos -= 1;
        if (stateinout.bufferPos < 0)
            stateinout.bufferPos = 0;
    }
    else if (key != "back" && key != "up" && key != "down" && key != "ctrl" && key != "tab" && key != "left" && key != "right")
    {
        for (int i = stateinout.bufferPos; *key; i++, key++)
        {
            val++;
            stateinout.buffer[i] = *key;
        }
        stateinout.bufferPos += val;
    }

    // hoh_debug("245_val: " << val);
    // hoh_debug("246_bufferPos: " << stateinout.bufferPos);
    // hoh_debug("247_buffer: " << stateinout.buffer);
}
// CUSTOM
int fibonacci(int n)
{
    if (n == 0)
    {
        return 0;
    }
    if (n == 1)
    {
        return 1;
    }
    int x = 0;
    int y = 1;
    for (int i = 2; i <= n; i++)
    {
        int temp = x;
        x = y;
        y += temp;
    }
    return y;
}

// CUSTOM
int fibonacci_Recursive(int n)
{
    if (n == 0)
    {
        return 0;
    }
    if (n == 1)
    {
        return 1;
    }
    return fibonacci_Recursive(n - 1) + fibonacci_Recursive(n - 2);
}

// CUSTOM
int factorial(int n)
{
    int ans = 1;
    for (int i = 2; i <= n; i++)
    {
        ans *= i;
    }
    return ans;
}

// CUSTOM
int rigourousTesting(int n)
{
    int ans = 1;
    for (int i = 2; i <= 200000000; i++)
    {
        ans = factorial(n);
        if (i % 100000 == 0)
        {
            // hoh_debug("i: " << i);
        }
    }
    return ans;
}

// Shell
void shell_step(shellstate_t &stateinout)
{
    if (stateinout.is_backgrounding || stateinout.sch_mode)
        return;
    int temp = stateinout.bufferPos;

    if (stateinout.bufferPos >= 4 && stateinout.buffer[temp - 1] == 'r' && stateinout.buffer[temp - 2] == 't' && stateinout.buffer[temp - 3] == 'n' && stateinout.buffer[temp - 4] == 'e')
    {
        stateinout.bufferPos -= 4;
        // hoh_debug("359_buffer: " << stateinout.buffer);

        if (temp >= 6 && stateinout.buffer[temp - 5] == 'c' && stateinout.buffer[temp - 6] == ' ')
        {
            if (stateinout.bufferPos >= 8 && stateinout.buffer[0] == 'r' && // Why 8?
                stateinout.buffer[1] == 't' && stateinout.buffer[2] == 'e' &&
                stateinout.buffer[3] == 's' && stateinout.buffer[4] == 't' && stateinout.buffer[5] == ' ')
            {
                stateinout.longComp_coroutine = true;
            }
            else
            {
                stateinout.longComp_coroutine = false;
            }

            stateinout.IsEnter = true;
            bool valid = true;
            uint32_t val = 0;

            for (int i = 6; i < stateinout.bufferPos - 2; i++)
            {
                if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                {
                    valid = false;
                    break;
                }
                val = val * 10 + (stateinout.buffer[i] - '0');
            }

            // hoh_debug("394_Val:" << val);

            if (valid)
            {
                if (val > 10)
                    val = 10;
                stateinout.coroutine_params[0] = val;
            }

            else
            {
                // hoh_debug("Invalid input ---------------------------");

                stateinout.longComp_coroutine = false;
            }

            return;
        }

        if (temp >= 6 && stateinout.buffer[temp - 6] == 'f' && stateinout.buffer[temp - 5] == 'r' && stateinout.buffer[temp - 7] == ' ')
        {
            if (stateinout.bufferPos >= 5 && stateinout.buffer[0] == 'f' && // Why 8?
                stateinout.buffer[1] == 'i' && stateinout.buffer[2] == 'b' &&
                stateinout.buffer[3] == ' ')
            {
                stateinout.longComp_fiber = true;
            }
            else
            {
                stateinout.longComp_fiber = false;
            }

            stateinout.IsEnter = true;

            bool valid = true;
            uint32_t val = 0;
            for (int i = 4; i < stateinout.bufferPos - 3; i++)
            {
                if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                {
                    valid = false;
                    break;
                }
                val = val * 10 + (stateinout.buffer[i] - '0');
            }

            // //hoh_debug("440_Val:" <<val);

            if (valid)
            {
                if (val > 10)
                    val = 10;
                stateinout.fiber_params[0] = val;
            }

            else
            {
                // hoh_debug("Invalid input ---------------------------");

                stateinout.longComp_fiber = false;
            }

            return;
        }

        if (temp >= 8 && stateinout.buffer[0] == 's' && stateinout.buffer[1] == 'e' && stateinout.buffer[2] == 'd' && stateinout.buffer[3] == ' ')
        {
            int st_i = 4;
            int prev_st = -1;
            bool valid = true;
            int cmd_count = 0;
            stateinout.fiber_number = 0;
            stateinout.sch_no = 0;
            int cmd_arr[3] = {0};
            int total_cmd = 0;
            bool valid2 = true;

            // hoh_debug("468_sed matched");

            while (st_i < stateinout.bufferPos)
            {
                if (stateinout.buffer[st_i] == 'f' &&
                    stateinout.buffer[st_i + 1] == 'i' && stateinout.buffer[st_i + 2] == 'b' &&
                    stateinout.buffer[st_i + 3] == ' ')
                {
                    prev_st = st_i + 4;
                    st_i += 4;
                    stateinout.fibr_inputs[cmd_count][0] = 'f';
                    stateinout.fibr_inputs[cmd_count][1] = 'i'; // ake
                    stateinout.fibr_inputs[cmd_count][2] = 'b';
                    if (cmd_arr[0] == 3 || total_cmd == 5)
                    {
                        // hoh_debug("unknown command 486");
                        char *p = "Same type max 3 and total of 5 can be called";

                        char *x = "";
                        for (int i = 0; i < stateinout.renderingStringSize; i++)
                        {
                            stateinout.renderingString[i] = *x;
                        }

                        for (int loc = 0; *p; loc++, p++)
                        {
                            stateinout.renderingString[loc] = *p;
                            stateinout.renderingStringSize += 1;
                        }

                        stateinout.longComp_fiber_s = false;
                        valid2 = false;
                        break;
                    }
                    else
                    {
                        cmd_arr[0]++;
                        total_cmd++;
                    }
                }
                else if (stateinout.buffer[st_i] == 'f' && stateinout.buffer[st_i + 1] == 'a' &&
                         stateinout.buffer[st_i + 2] == 'c' && stateinout.buffer[st_i + 3] == 't' &&
                         stateinout.buffer[st_i + 4] == ' ')
                {
                    prev_st = st_i + 5;
                    st_i += 5;
                    stateinout.fibr_inputs[cmd_count][0] = 'f';
                    stateinout.fibr_inputs[cmd_count][1] = 'a';
                    stateinout.fibr_inputs[cmd_count][2] = 'c';
                    stateinout.fibr_inputs[cmd_count][3] = 't';
                    if (cmd_arr[1] == 3 || total_cmd == 5)
                    {
                        // hoh_debug("unknown command 505");
                        char *p = "Same type max 3 and total of 5 can be called";

                        char *x = "";
                        for (int i = 0; i < stateinout.renderingStringSize; i++)
                        {
                            stateinout.renderingString[i] = *x;
                        }

                        for (int loc = 0; *p; loc++, p++)
                        {
                            stateinout.renderingString[loc] = *p;
                            stateinout.renderingStringSize += 1;
                        }

                        stateinout.longComp_fiber_s = false;
                        valid2 = false;
                        break;
                    }
                    else
                    {
                        cmd_arr[1]++;
                        total_cmd++;
                    }
                }
                else if (stateinout.buffer[st_i] == 'r' && stateinout.buffer[st_i + 1] == 't' &&
                         stateinout.buffer[st_i + 2] == 'e' && stateinout.buffer[st_i + 3] == 's' &&
                         stateinout.buffer[st_i + 4] == 't' && stateinout.buffer[st_i + 5] == ' ')
                {
                    prev_st = st_i + 6;
                    st_i += 6;

                    // hoh_debug("497_sed matched, rtest matched: ");
                    // hoh_debug("501 cmd_count:" << cmd_count);

                    for (int i = 0; i < 5; i++)
                    {
                        // hoh_debug("969_Fibr_inputs: " << stateinout.fibr_inputs[i]);
                        // hoh_debug("970_Fibr_outputs: " << stateinout.fibr_outputs[i]);
                    }

                    stateinout.fibr_inputs[cmd_count][0] = 'r';
                    stateinout.fibr_inputs[cmd_count][1] = 't';
                    stateinout.fibr_inputs[cmd_count][2] = 'e';
                    stateinout.fibr_inputs[cmd_count][3] = 's';
                    stateinout.fibr_inputs[cmd_count][4] = 't';

                    if (cmd_arr[2] == 3 || total_cmd == 5)
                    {
                        // hoh_debug("unknown command 542");
                        char *p = "Same type max 3 and total of 5 can be called";

                        char *x = "";
                        for (int i = 0; i < stateinout.renderingStringSize; i++)
                        {
                            stateinout.renderingString[i] = *x;
                        }

                        for (int loc = 0; *p; loc++, p++)
                        {
                            stateinout.renderingString[loc] = *p;
                            stateinout.renderingStringSize += 1;
                        }

                        stateinout.longComp_fiber_s = false;
                        valid2 = false;
                        break;
                    }
                    else
                    {
                        cmd_arr[2]++;
                        total_cmd++;
                    }

                    for (int i = 0; i < 5; i++)
                    {
                        // hoh_debug("969_Fibr_inputs: " << stateinout.fibr_inputs[i]);
                        // hoh_debug("970_Fibr_outputs: " << stateinout.fibr_outputs[i]);
                        //  render.fibr_inputs[i] = shell.fibr_inputs[i];
                        //  render.fibr_outputs[i] = shell.fibr_outputs[i];
                    }
                }
                else if(stateinout.buffer[st_i] != ';' && (stateinout.buffer[st_i] < '0' || stateinout.buffer[st_i] > '9'))
                {
                    valid2 = false;
                    stateinout.longComp_fiber_s = false;
                    break;
                }

                if (stateinout.buffer[st_i] == ';')
                {
                    uint32_t val = 0;
                    for (int i = prev_st; i < st_i; i++)
                    {
                        if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                        {
                            // hoh_debug_nl('buffer[i]' << stateinout.buffer[i]);
                            valid = false;
                            break;
                        }
                        val = val * 10 + (stateinout.buffer[i] - '0');
                    }
                    if (!valid)
                    {
                        break;
                    }
                    else
                    {
                        stateinout.fibr_params[cmd_count] = val;
                        stateinout.done_fiber_array[cmd_count] = false;
                        cmd_count++;
                    }
                    st_i++;
                }
                else
                {
                    st_i++;
                }
                // else{
                //     valid = false;
                //     break;
                // }
            }
            stateinout.IsEnter = true;
            if (valid && valid2)
            {
                stateinout.sch_no = cmd_count;
                stateinout.longComp_fiber_s = true;
            }
            else
            {
                // hoh_debug("Invalid input sed ---------------------------");
                stateinout.longComp_fiber_s = false;
            }
            // hoh_debug("return_s-------------------------");
            if (valid2 == true)
            {
                return;
            }
        }

        if (temp >= 6 && stateinout.buffer[temp - 5] == 'f' && stateinout.buffer[temp - 6] == ' ')
        {
            if (stateinout.bufferPos >= 8 && stateinout.buffer[0] == 'r' && // Why 8?
                stateinout.buffer[1] == 't' && stateinout.buffer[2] == 'e' &&
                stateinout.buffer[3] == 's' && stateinout.buffer[4] == 't' && stateinout.buffer[5] == ' ')
            {
                stateinout.longComp_fiber = true;
            }
            else
            {
                stateinout.longComp_fiber = false;
            }

            stateinout.IsEnter = true;

            bool valid = true;
            uint32_t val = 0;

            // Look for space between fact i.e. function and argument
            for (int i = 6; i < stateinout.bufferPos - 2; i++)
            {
                if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                {
                    valid = false;
                    break;
                }
                val = val * 10 + (stateinout.buffer[i] - '0');
            }

            // //hoh_debug("440_Val:" <<val);

            if (valid)
            {
                if (val > 10)
                    val = 10;
                stateinout.fiber_params[0] = val;
            }

            else
            {
                // hoh_debug("Invalid input ---------------------------");

                stateinout.longComp_fiber = false;
            }

            return;
        }

        // stateinout.out_buffer[stateinout.cursorLine++] = stateinout.buffer;
        if (stateinout.bufferPos >= 4 && stateinout.buffer[0] == 'e' && stateinout.buffer[1] == 'c' && stateinout.buffer[2] == 'h' && stateinout.buffer[3] == 'o')
        {
            for (int i = 5; i < stateinout.bufferPos; i++)
            {
                stateinout.renderingStringSize += 1;
                stateinout.renderingString[i - 5] = stateinout.buffer[i];
            }
        }
        else if (stateinout.bufferPos >= 4 && stateinout.buffer[0] == 'f' && stateinout.buffer[1] == 'i' && stateinout.buffer[2] == 'b' && stateinout.buffer[3] == ' ')
        {
            bool valid = true;
            uint32_t val = 0;
            for (int i = 4; i < stateinout.bufferPos; i++)
            {
                if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                {
                    valid = false;
                    break;
                }
                val = val * 10 + (stateinout.buffer[i] - '0');
            }

            if (!valid)
            {
                // hoh_debug("Invalid input");
            }
            else
            {
                int ans = fibonacci_Recursive(val);
                // int ans = fibonacci(val);

                // hoh_debug("ANS: " << ans);

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
                    // stateinout.bufferPos = 0;
                }
            }
        }

        else if (stateinout.bufferPos >= 4 && stateinout.buffer[0] == 'e' && stateinout.buffer[1] == 'x' && stateinout.buffer[2] == 'i' && stateinout.buffer[3] == 't')
        {
            char *p = "Press Ctrl+Alt+Q";
            for (int loc = 0; *p; loc++, p++)
            {
                stateinout.renderingString[loc] = *p;
                stateinout.renderingStringSize += 1;
            }
        }

        // Look for space between fact i.e. function and argument
        else if (stateinout.bufferPos >= 5 && stateinout.buffer[0] == 'f' && stateinout.buffer[1] == 'a' && stateinout.buffer[2] == 'c' && stateinout.buffer[3] == 't' && stateinout.buffer[4] == ' ')
        {
            bool valid = true;
            uint32_t val = 0;

            // Look for space between fact i.e. function and argument
            for (int i = 5; i < stateinout.bufferPos; i++)
            {
                if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                {
                    valid = false;
                    break;
                }
                val = val * 10 + (stateinout.buffer[i] - '0');
            }
            // hoh_debug("VAL: " << val);
            if (!valid)
            {
                // hoh_debug("Invalid input");
                //  stateinout.renderingString = "Invalid input";
            }
            else
            {
                int ans = factorial(val);

                // hoh_debug("ANS: " << ans);

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
            }
        }

        else if (stateinout.bufferPos >= 6 && stateinout.buffer[0] == 'r' && stateinout.buffer[1] == 't' && stateinout.buffer[2] == 'e' && stateinout.buffer[3] == 's' && stateinout.buffer[4] == 't' && stateinout.buffer[5] == ' ')
        {
            bool valid = true;
            uint32_t val = 0;

            // Look for space between fact i.e. function and argument
            for (int i = 6; i < stateinout.bufferPos; i++)
            {
                if (stateinout.buffer[i] < '0' || stateinout.buffer[i] > '9')
                {
                    valid = false;
                    break;
                }
                val = val * 10 + (stateinout.buffer[i] - '0');
            }
            // hoh_debug("VAL: " << val);
            if (!valid)
            {
                // hoh_debug("Invalid input");
                //  stateinout.renderingString = "Invalid input";
            }
            else
            {
                if (val > 10)
                    val = 10;
                int ans = rigourousTesting(val);

                // hoh_debug("ANS: " << ans);

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
            }
        }

        else if (stateinout.bufferPos == 2 && stateinout.buffer[0] == 'u' && stateinout.buffer[1] == 'p')
        {
            // Move up
            stateinout.RenderShell_movement = 0;
        }
        else if (stateinout.bufferPos == 4 && stateinout.buffer[0] == 'd' && stateinout.buffer[1] == 'o' && stateinout.buffer[2] == 'w' && stateinout.buffer[3] == 'n')
        {
            // Move down
            stateinout.RenderShell_movement = 1;
        }
        else
        {
            // hoh_debug("unknown command");

            char *p = "Unknown command";

            char *x = "";
            for (int i = 0; i < stateinout.renderingStringSize; i++)
            {
                stateinout.renderingString[i] = *x;
            }

            for (int loc = 0; *p; loc++, p++)
            {
                stateinout.renderingString[loc] = *p;
                stateinout.renderingStringSize += 1;
            }

            // hoh_debug("Rendering String (Unknowm): " << stateinout.renderingString);
        }

        stateinout.IsEnter = true;

        // hoh_debug("932_Rendering string: " << stateinout.renderingString);
        // hoh_debug("933_Rendering string size: " << stateinout.renderingStringSize);

        for (int i = 0; i < stateinout.renderingStringSize; i++)
        {
            stateinout.CommandOutput[i] = stateinout.renderingString[i];
        }

        for (int i = 0; i < stateinout.bufferPos; i++)
        {
            stateinout.ShellCommand[i] = stateinout.buffer[i];
        }

        // hoh_debug("CMDOUTPUT: " << stateinout.CommandOutput);

        for (int i = 0; i < stateinout.bufferPos; i++)
        {
            stateinout.PreviousCommand[i] = stateinout.buffer[i];
        }

        char *x = "";
        for (int i = 0; i < stateinout.renderingStringSize; i++)
        {
            stateinout.renderingString[i] = *x;
        }

        stateinout.renderingStringSize = 0;

        for (int i = 0; i < stateinout.bufferPos; i++)
        {
            stateinout.buffer[i] = *x;
        }

        /////////////////////////
        stateinout.bufferPos = 0;
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
        // //hoh_debug("CMD1: " << stateinout.PreviousCommand);
        // //hoh_debug("CMD2: " << stateinout.CommandOutput);
        // hoh_debug("-------------------BEFORE-----------");
        for (int i = 0; i < 6; i++)
        {
            // hoh_debug("CMDIN: " << stateinout.last_commands[i]);
            // hoh_debug("CMDOUT: " << stateinout.last_outputs[i]);
        }

        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            stateinout.last_commands[stateinout.totalcommands - 1][j] = stateinout.PreviousCommand[j];
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            stateinout.last_outputs[stateinout.totalcommands - 1][j] = stateinout.CommandOutput[j];
        // char *x = "";
        // hoh_debug("-------------------AFTER-----------");

        for (int i = 0; i < 6; i++)
        {
            // hoh_debug("CMDIN: " << stateinout.last_commands[i]);
            // hoh_debug("CMDOUT: " << stateinout.last_outputs[i]);
        }
        for (int i = 0; i < 100; i++)
        {
            stateinout.ShellCommand[i] = *x;
        }
        for (int i = 0; i < 100; i++)
        {
            stateinout.CommandOutput[i] = *x;
        }
        for (int i = 0; i < 100; i++)
        {
            stateinout.PreviousCommand[i] = *x;
        }
    } // If condition of IsEnter ends here
    else
    {
        stateinout.IsEnter = false;
        for (int i = 0; i < stateinout.bufferPos; i++)
        {
            stateinout.ShellCommand[i] = stateinout.buffer[i];
        }
        char *x = "";
        for (int i = stateinout.bufferPos; i < SHELL_BUFFER_LEN; i++)
        {
            stateinout.ShellCommand[i] = *x;
        }
        // //hoh_debug("Buffer Command: " << stateinout.buffer);
        // //hoh_debug("Shell Command: " << stateinout.ShellCommand);
    }
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x, int y, const char *str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x, int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);

//
// Given a render state, we need to write it into vgatext buffer
//
static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
    vgatext::writechar(y * w + x, c, bg, fg, vgatext_base);
    // loc, char,
}

//
// shellstate --> renderstate
//
void shell_render(const shellstate_t &shell, renderstate_t &render)
{
    if (shell.sch_mode)
    {
        // hoh_debug("--------Inside Shell Render: sch_mode"
        //                  << "True");
    }

    render.numKeysPressed = shell.numKeysPressed;
    render.IsEnter = shell.IsEnter;
    render.totalcommands = shell.totalcommands;

    render.rendercheck = shell.rendercheck;

    render.sch_mode = shell.sch_mode;
    render.sch_no = shell.sch_no;

    for (int i = 0; i < 5; i++)
    {
        render.got_output[i] = shell.got_output[i];
    }
    if (shell.IsEnter)
    {
        // hoh_debug("Shell Enter is "
        //<< "True");
    }
    for (int j = 0; j < SHELL_BUFFER_SIZE; j++)
        render.curr_command[j] = shell.ShellCommand[j];
    for (int i = 0; i < SHELL_BUFFER_SIZE; i++)
    {
        render.CommandOutput[i] = shell.CommandOutput[i];
    }
    // //hoh_debug("COpying cmd out" << render.CommandOutput);
    // TODO instead of writing to fibr_output only, write to CommandOutput too

    if (render.IsEnter)
    {
        // hoh_debug("SR\tTotalcommands:" << render.totalcommands);
        // hoh_debug("SR\tKey Presses:" << shell.numKeysPressed);
        // hoh_debug("SR\tcurrent command:" << shell.ShellCommand);
        // hoh_debug("SR\tcurrent output:" << shell.CommandOutput);
        // hoh_debug("SR\tPrevious Command:" << shell.PreviousCommand);
        // hoh_debug("SR: IsEnter"<< "True");
    }
    int i;
    for (i = 0; i < 6; i++)
    {
        for (int j = 0; j < SHELL_BUFFER_LEN; j++) // hello?
            render.last_commands[i][j] = shell.last_commands[i][j];
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            render.last_outputs[i][j] = shell.last_outputs[i][j];
    }

    if (render.IsEnter && render.rendercheck != true && !render.sch_mode)
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            render.curr_command[j] = ' ';

    // if (render.sch_mode)
    // hoh_debug("--------Exiting Shell Render: sch_mode"<< "True");
}
//
// compare a and b
//
bool render_eq(const renderstate_t &a, const renderstate_t &b)
{
    if (a.sch_mode && !b.sch_mode)
        return false;
    if (a.rendercheck != b.rendercheck)
        return false;
    if (a.numKeysPressed == b.numKeysPressed)
        // if (a.numKeysPressed == b.numKeysPressed && b.rendercheck)
        return true;
    return false;
}

int get_length_shell(char *a)
{
    int i = 0;
    while (a[i] != '\0')
        i++;
    return i;
}
void clear_lines(int line, int w, int h, addr_t vgatext_base)
{
    uint8_t bg = 0, fg = 7;
    char *x = "";
    for (int j = 12; j < 28; j++)
        for (int off = 0; off < w; off++)
            writecharxy(off, j, *x, bg, fg, w, h, vgatext_base);
}

void render(const renderstate_t &state, int w, int h, addr_t vgatext_base)
{
    uint8_t bg = 0, fg = 7;
    uint8_t black = 0;
    const char *p = "KeyPresses: ";
    drawtext(0, 0, p, w, bg, 0xe, w, h, vgatext_base);
    drawnumberinhex(14, 0, state.numKeysPressed, w, bg, 0xe, w, h, vgatext_base);
    int rec[4] = {0, 1, w - 1, 12};
    drawrect(rec[0], rec[1], rec[2], rec[3], bg, 0xb, w, h, vgatext_base);
    int line = 2;
    // hoh_debug("key ye tto be oressed");
    // hoh_debug("------------------------------");
    //  {
    //  const char *p1 = "Menu: ";
    //  drawtext(1, line++, p1, w, bg, 0x5, w, h, vgatext_base);
    const char *p2 = "1: echo <text>";
    drawtext(1, line++, p2, w, bg, 0x5, w, h, vgatext_base);
    const char *p3 = "2: fib <num>";
    drawtext(1, line++, p3, w, bg, 0x5, w, h, vgatext_base);
    const char *p4 = "3: fact <num> {Max num:16}";
    drawtext(1, line++, p4, w, bg, 0x5, w, h, vgatext_base);
    const char *p5 = "4: rtest <num> {Max num:10}";
    drawtext(1, line++, p5, w, bg, 0x5, w, h, vgatext_base);
    const char *p6 = "5: rtest <num> c{Max num:10} :coroutine";
    drawtext(1, line++, p6, w, bg, 0x5, w, h, vgatext_base);
    const char *p7 = "6: rtest <num> f{Max num:10} :fiber";
    drawtext(1, line++, p7, w, bg, 0x5, w, h, vgatext_base);
    const char *p8 = "7: sed <rtest/fib/fact <num>;>{Max 5 commands, Max num:10} :scheduler fiber";
    drawtext(1, line++, p8, w, bg, 0x5, w, h, vgatext_base);
    const char *p9 = "Ex of 7: sed rtest 2;rtest 4;fib 10;fact 2;fact 3;";
    drawtext(1, line++, p9, w, bg, 0x5, w, h, vgatext_base);
    const char *p10 = "8: exit ";
    drawtext(1, line++, p10, w, bg, 0x5, w, h, vgatext_base);

    line = 12;
    clear_lines(line, w, h, vgatext_base);
    const char *dollar = "os-user@oslab:$";
    uint16_t offset = sizeof(dollar) * 4 + 1;
    offset = 17; // For testing
    drawtext(1, line, dollar, w, bg, 0x2, w, h, vgatext_base);
    hoh_debug("Printing sch_mode: inside render: " << state.sch_mode);
    hoh_debug("Writing curr command on " << line);
    for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        writecharxy(offset + j, line, state.curr_command[j], bg, fg, w, h, vgatext_base);
    line++;
    // }
    int off = 0;
    if (state.sch_mode)
    {
        hoh_debug("COmmandoutiNSide" << state.CommandOutput);
        drawtext(1, line, "> ", w, bg, fg, w, h, vgatext_base);
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        {
            if (state.CommandOutput[j] == '/')
            {
                line++;
                off = 0;
                continue;
            }
            writecharxy(3 + off++, line, state.CommandOutput[j], bg, fg, w, h, vgatext_base);
        }
    }
    line++;
    // hoh_debug("----------End of the rendering");
    hoh_debug("Writing prev command on " << line);
    for (int i = state.totalcommands - 1; i >= 0; i--)
    {
        // hoh_debug("tot------------------------------");
        // hoh_debug(state.last_commands[i]);
        // hoh_debug(state.last_outputs[i]);
        drawtext(1, line, dollar, w, bg, 0x2, w, h, vgatext_base);
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            writecharxy(offset + j, line, state.last_commands[i][j], bg, fg, w, h, vgatext_base);
        line++;
        drawtext(1, line, "> ", w, bg, fg, w, h, vgatext_base);
        int off = 0;
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        {
            if (state.last_outputs[i][j] == '/')
            {
                line++;
                off = 0;
                continue;
            }
            writecharxy(3 + off++, line, state.last_outputs[i][j], bg, fg, w, h, vgatext_base);
        }
        // if (off)
        //     line--;
        line++;
    }
}
void render2(const renderstate_t &state, int w, int h, addr_t vgatext_base)
{
    // hoh_debug("--------Inside Render Function");
    // hoh_debug("R\tTotal Commands:" << state.totalcommands);
    // hoh_debug("R\tKey Presses:" << state.numKeysPressed);
    // hoh_debug("R\tcurrent command:" << state.curr_command);

    uint8_t bg = 0, fg = 7;
    uint8_t black = 0;

    const char *p = "KeyPresses: ";

    drawtext(0, 1, p, w, bg, 0xe, w, h, vgatext_base);
    drawnumberinhex(14, 1, state.numKeysPressed, w, bg, 0xe, w, h, vgatext_base);
    int rec[4] = {0, 2, w - 1, 12};
    drawrect(rec[0], rec[1], rec[2], rec[3], bg, 0xb, w, h, vgatext_base);
    int line = 3;
    // hoh_debug("key ye tto be oressed");
    // hoh_debug("------------------------------");
    //  {
    const char *p1 = "Menu: ";
    drawtext(1, line++, p1, w, bg, 0x5, w, h, vgatext_base);
    const char *p2 = "1: echo <text>";
    drawtext(1, line++, p2, w, bg, 0x5, w, h, vgatext_base);
    const char *p3 = "2: fibonacci <num>";
    drawtext(1, line++, p3, w, bg, 0x5, w, h, vgatext_base);
    const char *p4 = "3: factorial <num> {Max Val:16}";
    drawtext(1, line++, p4, w, bg, 0x5, w, h, vgatext_base);
    const char *p5 = "4: rtest <num> {Max Val:10}";
    drawtext(1, line++, p5, w, bg, 0x5, w, h, vgatext_base);
    const char *p55 = "5: rtest <num> c{Max Val:10} :coroutine";
    drawtext(1, line++, p55, w, bg, 0x5, w, h, vgatext_base);
    const char *p6 = "6: exit ";
    drawtext(1, line++, p6, w, bg, 0x5, w, h, vgatext_base);

    line = 12;
    // //hoh_debug("1------------------------------");
    const char *dollar = "os-user@oslab:$";
    uint16_t offset = sizeof(dollar) * 4 + 1;
    offset = 17; // For testing
    for (int i = 0; i < state.totalcommands; i++)
    {
        // hoh_debug("tot------------------------------");
        // hoh_debug(state.last_commands[i]);
        // hoh_debug(state.last_outputs[i]);
        drawtext(1, line, dollar, w, bg, 0x2, w, h, vgatext_base);
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
            writecharxy(offset + j, line, state.last_commands[i][j], bg, fg, w, h, vgatext_base);
        line++;
        drawtext(1, line, "> ", w, bg, fg, w, h, vgatext_base);
        int off = 0;
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        {
            if (state.last_outputs[i][j] == '/')
            {
                line++;
                off = 0;
                continue;
            }
            writecharxy(3 + off++, line, state.last_outputs[i][j], bg, fg, w, h, vgatext_base);
        }
        // if (off)
        //     line--;
        line++;
    }
    drawtext(1, line, dollar, w, bg, 0x2, w, h, vgatext_base);
    // hoh_debug("Printing sch_mode: inside render: " << state.sch_mode);

    for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        writecharxy(offset + j, line, state.curr_command[j], bg, fg, w, h, vgatext_base);
    line++;
    // }
    int off = 0;
    if (state.sch_mode)
    {
        // hoh_debug("COmmandoutiNSide" << state.CommandOutput);
        drawtext(1, line, "> ", w, bg, fg, w, h, vgatext_base);
        for (int j = 0; j < SHELL_BUFFER_LEN; j++)
        {
            if (state.CommandOutput[j] == '/')
            {
                line++;
                off = 0;
                continue;
            }
            writecharxy(3 + off++, line, state.CommandOutput[j], bg, fg, w, h, vgatext_base);
        }
    }
    line++;
    // hoh_debug("----------End of the rendering");
}

// helper functions

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
    for (int y = y0; y < y1; y++)
    {
        for (int x = x0; x < x1; x++)
        {
            writecharxy(x, y, 0, bg, fg, w, h, vgatext_base);
        }
    }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{

    writecharxy(x0, y0, 0xc9, bg, fg, w, h, vgatext_base);
    writecharxy(x1 - 1, y0, 0xbb, bg, fg, w, h, vgatext_base);
    writecharxy(x0, y1 - 1, 0xc8, bg, fg, w, h, vgatext_base);
    writecharxy(x1 - 1, y1 - 1, 0xbc, bg, fg, w, h, vgatext_base);

    for (int x = x0 + 1; x + 1 < x1; x++)
    {
        writecharxy(x, y0, 0xcd, bg, fg, w, h, vgatext_base);
    }

    for (int x = x0 + 1; x + 1 < x1; x++)
    {
        writecharxy(x, y1 - 1, 0xcd, bg, fg, w, h, vgatext_base);
    }

    for (int y = y0 + 1; y + 1 < y1; y++)
    {
        writecharxy(x0, y, 0xba, bg, fg, w, h, vgatext_base);
    }

    for (int y = y0 + 1; y + 1 < y1; y++)
    {
        writecharxy(x1 - 1, y, 0xba, bg, fg, w, h, vgatext_base);
    }
}

static void drawtext(int x, int y, const char *str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
    for (int i = 0; i < maxw; i++)
    {
        writecharxy(x + i, y, str[i], bg, fg, w, h, vgatext_base);
        if (!str[i])
        {
            break;
        }
    }
}

static char int2char(unsigned int i)
{
    return "0123456789"[i % 10];
}
static void num2char(unsigned int i, char str[])
{
    uint max = sizeof(uint32_t) * 2 + 1;
    char a[max];
    for (int i = 0; i < max - 1; i++)
    {
        a[max - 1 - i - 1] = int2char(i % 10);
        i = i / 10;
    }
    a[max - 1] = '\0';
}
static void drawnumberinhex(int x, int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
    enum
    {
        max = sizeof(uint32_t) * 2 + 1
    };
    char a[max];
    for (int i = 0; i < max - 1; i++)
    {
        a[max - 1 - i - 1] = hex2char(number % 16);
        number = number / 16;
    }
    a[max - 1] = '\0';

    drawtext(x, y, a, maxw, bg, fg, w, h, vgatext_base);
}
