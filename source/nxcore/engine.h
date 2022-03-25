#ifndef NINETAILSX_GAME_H
#define NINETAILSX_GAME_H
#include <nxcore/primitives.h>
#include <nxcore/memory.h>
#include <nxcore/math.h>
#include <nxcore/input.h>
#include <nxcore/resources.h>
#include <nxcore/renderer.h>

typedef struct
{
	btmonotonic_memory_arena EngineMemoryArena;
	b32 Initialized;

	i32 x, y;
	b32 mov_flip;

	// State for the testbitmap.
	void* testbitmap_res;
	dibitmap testbitmap;

	dibitmap base_layer;

} engine_state;

#endif
