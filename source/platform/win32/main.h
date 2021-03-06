#ifndef NINETAILSX_WIN32_MAIN_H
#define NINETAILSX_WIN32_MAIN_H
#include <windows.h>
#include <nxcore/core.h>


typedef struct engine_library
{
	fnptr_engine_init* EngineInit;
	fnptr_engine_reinit* EngineReinit;
	fnptr_engine_runtime* EngineRuntime;
} engine_library;

typedef struct app_state
{
	res_handler_interface ResourceHandlerInterface;
	engine_library EngineLibrary;
	window_props WindowProperties;
	void* appMemStore;
	u64 appMemSize;
	action_interface InputHandle;
	input InputSwapBuffer[2];
	char BasePath[MAX_PATH];
	u64 PerformanceFrequency;
	HDC WindowDeviceContext;
	b32 isRunnning;
} app_state;

/**
 * The application state functions as a global variable, so we are going to define a pointer
 * reference to it here which will guarantee its existence at the start of main.
 */
app_state* ApplicationState;

#endif