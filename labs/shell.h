#pragma once
#include "util/config.h"
#include "util/debug.h"
// #include "labs/coroutine.h"
// #include "util/coroutine.h"

#define SHELL_BUFFER_SIZE 100
#define SHELL_BUFFER_LEN 50

struct shellstate_t
{
    uint32_t numKeysPressed = 0;
    char buffer[SHELL_BUFFER_SIZE];
    int bufferSize;
    int bufferPos;
    int cursorLine;
    char *KeysPresses[100];

    char *keys[100];
    char *scancode[100];
    bool IsEnter = false;
    int RenderShell_movement = -1; // 0 for up and 1 for down
    char renderingString[100];
    int renderingStringSize = 0;
    int totalcommands = 0;
    char ShellCommand[100];
    char CommandOutput[100];
    char PreviousCommand[100];

    int CommandOutputSize;
    /*char *ShellCommand;
    char *CommandOutput;
    char *PreviousCommand;*/

    void increment_key_presses()
    {
        numKeysPressed++;
    }
    char *scancode_to_key(uint8_t scankey);
    int key_to_scancode(uint8_t key);
    bool out_ready = false;
    char out_buffer[SHELL_BUFFER_SIZE][SHELL_BUFFER_LEN];
    int out_buffer_size = 0;
    char curr_command[SHELL_BUFFER_LEN];
    char last_commands[7][SHELL_BUFFER_LEN];
    char last_outputs[7][SHELL_BUFFER_LEN];

    // coroutine
    int coroutine_params[100];
    bool longComp_coroutine = false;
    bool is_backgrounding = false;
    bool rendercheck = false;
    // int ans;

    // fiber
    bool done_fiber = false;
    int ret_fiber;
    int fiber_params[100];
    bool longComp_fiber = false;

    // scheduler
    bool done_fiber_array[5] = {false, false, false, false, false};
    int ret_fiber_array[5];
    int fiber_number = 0;
    bool longComp_fiber_s = false;

    // };

    // array of length 10
    // i   -> function name
    // i+1 -> function argument

    // fibr_inputs  -> {rtest, rtest, func_name, ...}
    // fibr_params  -> {10, 15, 20, ...}

    // fibr_outputs_size -> {-1, -1, -1, -1, -1}

    char fibr_inputs[5][SHELL_BUFFER_LEN];
    char fibr_outputs[5][SHELL_BUFFER_LEN];
    // char *fibr_inputs[5];
    // char *fibr_outputs[5];
    int fibr_params[5];
    int fibr_outputs_size[5];
    bool got_output[5] = {false, false, false, false, false};
    bool backgrounding_sch[5] = {false, false, false, false, false};
    int sch_no = 0;
    bool sch_mode = false; // Ise true kr dena jb schedule mode me ho else false @abhinav kumar @prakhar aggarwal
    bool prat_flag = false;
};
struct renderstate_t
{
    int cursorLine;
    int bufferPos = 0;
    bool IsEnter = false;
    char renderingString[100];
    char buffer[SHELL_BUFFER_SIZE];
    int renderingStringSize = 0;
    char out_buffer[SHELL_BUFFER_SIZE][SHELL_BUFFER_LEN];
    bool out_ready = false;
    uint32_t numKeysPressed = 0;
    int totalcommands;
    char curr_command[SHELL_BUFFER_LEN];
    char last_commands[7][SHELL_BUFFER_LEN];
    char last_outputs[7][SHELL_BUFFER_LEN];
    bool rendercheck = false;
    char CommandOutput[100];
    // int sch_no = 0;
    // bool sch_mode = false;

    // char *fibr_inputs[5];
    // char *fibr_outputs[5];
    char fibr_inputs[5][SHELL_BUFFER_LEN];
    char fibr_outputs[5][SHELL_BUFFER_LEN];
    int sch_no = 0;
    bool sch_mode = false;
    bool got_output[5] = {false};
    ///////////////////////////
    bool prat_flag = false;
};

void shell_init(shellstate_t &state);
void shell_update(uint8_t scankey, shellstate_t &stateinout);
void shell_step(shellstate_t &stateinout);
void shell_render(const shellstate_t &shell, renderstate_t &render);

bool render_eq(const renderstate_t &a, const renderstate_t &b);
void render(const renderstate_t &state, int w, int h, addr_t display_base);
