#ifndef NINETAILSX_GAME_H
#define NINETAILSX_GAME_H
#include <nxcore/primitives.h>
#include <nxcore/input.h>

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
	i32 Width;
	i32 Height;
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

/** Platform -> Engine */
typedef u32 fnptr_platform_fetch_res_file(char* RelativePath, void* Buffer, u32 BuffSize);
typedef u32 fnptr_platform_fetch_res_size(char* RelativePath);

/** Engine -> Platform */
typedef i32 fnptr_engine_init(memory_layout* MemoryLayout);
typedef i32 fnptr_engine_reinit(memory_layout* MemoryLayout);
typedef i32 fnptr_engine_runtime(memory_layout* MemoryLayout, renderer* Renderer, action_interface* InputHandle);

#endif
