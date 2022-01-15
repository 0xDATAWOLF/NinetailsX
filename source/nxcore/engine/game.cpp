#include "game.h"
#include <windows.h>

extern "C" __declspec(dllexport) i32
EngineRuntime()
{

	MessageBoxA(0, "This is from EngineRuntime!", "Debug", MB_OK);

	return 0;
}