#include "common.h"
#include "cmd.h"
#include "window.h"
#include "input.h"
#include "client.h"
#include "server.h"
#include "stdlib.h"
#define HELL_SIMPLE_TYPE_NAMES
#include "types.h"

typedef Hell_C_Var Var;

Hell_FrameFn    userFrame;
Hell_ShutDownFn userShutDown;

static void dummyUserFrame(void)
{
    // no op we call in case user frame is not provided
    // may be an optimization? gets rid of an if statement in the loop
    // because we always call something
}

void hell_Init(bool initConsole, Hell_FrameFn fn1, Hell_ShutDownFn fn2, const Hell_Window** window)
{
    if (fn1)
        userFrame = fn1;
    else
        userFrame = dummyUserFrame;
    userShutDown = fn2;
    hell_c_Init();
    hell_c_AddCommand("quit", hell_Quit);
    hell_i_Init(initConsole);
    const Hell_C_Var* varW = hell_c_GetVar("d_width", "600", HELL_C_VAR_ARCHIVE_BIT);
    const Hell_C_Var* varH = hell_c_GetVar("d_height", "500", HELL_C_VAR_ARCHIVE_BIT);
    const Hell_C_Var* dedicated = hell_c_GetVar("dedicated", "0", 0);
    if (window)
        *window = hell_w_Init(varW->value, varH->value, NULL);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Initialized.\n");
}

void hell_Frame(Tick delta)
{
    hell_i_DrainEvents();
    hell_i_PumpEvents();
    hell_c_Execute();
}

void hell_Loop(void)
{
    const Var* vFps = hell_c_GetVar("maxFps", "60", HELL_C_VAR_ARCHIVE_BIT);
    const double targetFrameLength = (1.0 / vFps->value);
    Tick startTick = hell_Time(); 
    Tick endTick = startTick;
    hell_Announce("Entering Hell Loop.\n");
    while (1)
    {
        // this is just a place holder
        hell_Sleep(targetFrameLength);
        endTick = hell_Time();
        hell_Frame(endTick - startTick);
        userFrame();
        startTick = endTick;
    }
}

void hell_ShutDown(void)
{
    hell_w_CleanUp();
    hell_i_CleanUp();
    hell_Announce("Shut Down.\n");
}

void hell_Quit(void)
{
    if (userShutDown)
        userShutDown();
    hell_ShutDown();
    exit(0);
}