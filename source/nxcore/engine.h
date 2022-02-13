#ifndef NINETAILSX_GAME_H
#define NINETAILSX_GAME_H
#include <nxcore/primitives.h>
#include <nxcore/memory.h>
#include <nxcore/math.h>
#include <nxcore/input.h>

typedef u32 fnptr_platform_fetch_res_file(char* RelativePath, void* Buffer, u32 BuffSize);
typedef u32 fnptr_platform_fetch_res_size(char* RelativePath);

typedef struct res_handler_interface
{
	fnptr_platform_fetch_res_file* FetchResourceFile;
	fnptr_platform_fetch_res_size* FetchResourceSize;
} res_handler_interface;

/**
 * renderer
 * 			The render layout which serves as an in/out method of providing rendering data to the
 * 			platform. The platform is responsible for reporting the current width/height of the
 * 			drawing area/client area, while the engine is responsible for providing the image data
 * 			back to the platform. The engine may resize the window by setting the width/height and
 * 			the platform must respond to these changes by setting the window size.
 * 
 */
typedef struct renderer
{
	v2i WindowDimensions;
	void* Image;
} renderer;

/**
 * memory_layout
 * 			The memory layout that is provided by the platform which details the heap memory layout.
 */
typedef struct memory_layout
{
	void* Base;
	u64 Size;
} memory_layout;

/**
 * engine_state
 * 			Maintains the engine state which persists throughout the lifetime of the application runtime.
 */
typedef struct engine_state
{
	btmonotonic_memory_arena EngineMemoryArena;
	b32 Initialized;

	i32 x, y;
	b32 mov_flip;

} engine_state;


/** Engine -> Platform */
typedef i32 fnptr_engine_init(memory_layout* MemoryLayout, renderer* Renderer, res_handler_interface* ResHandler);
typedef i32 fnptr_engine_reinit(memory_layout* MemoryLayout, renderer* Renderer, res_handler_interface* ResHandler);
typedef i32 fnptr_engine_runtime(memory_layout* MemoryLayout, renderer* Renderer, action_interface* InputHandle);

#endif
