#include <stdio.h>
#include <unistd.h>
#include "cmd.h"
#include "input.h"
#include "common.h"
#include "window.h"
#include "platform.h"
#ifdef WINDOWS
#include "win_local.h"
#endif

static void chungus(void)
{
    hell_Print("Aw lawd %s comin!\n", hell_c_Argv(1));
}

void hell_Main(void)
{
    const Hell_Window* window = NULL;
#if defined(UNIX)
    hell_Init(true, NULL, NULL, &window);
#elif defined(WINDOWS)
    hell_Init(false, NULL, NULL, &window);
#endif
    hell_c_AddCommand("chungus", chungus);
    hell_Loop();
}

#if defined(UNIX)
int main(int argc, char *argv[])
{
    hell_Main();
}
#elif defined(WINDOWS)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    winVars.instance = hInstance;
    hell_Main();
    return 0;
}

#endif
