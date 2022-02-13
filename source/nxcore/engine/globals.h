#ifndef NINETAILSX_GLOBALS_H
#define NINETAILSX_GLOBALS_H
#include <nxcore/engine.h>

fnptr_platform_fetch_res_file* FetchResourceFile;
fnptr_platform_fetch_res_size* FetchResourceSize;

global engine_state* EngineState;
global memory_layout* EngineMemoryLayout;
global renderer* EngineRenderer;

#endif