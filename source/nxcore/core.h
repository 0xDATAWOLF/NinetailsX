/**
 * Included on the platform in order to complete the link between
 * the engine and platform.
 */
#ifndef NINETAILSX_CORE_H
#define NINETAILSX_CORE_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>
#include <nxcore/input.h>
#include <nxcore/resources.h>

typedef struct
{
	v2i dimensions;
	void* softwareBitmap;
} window_props;

/** Platform -> Engine */
typedef u32 fnptr_platform_fetch_res_file(char* RelativePath, void* Buffer, u32 BuffSize);
typedef u32 fnptr_platform_fetch_res_size(char* RelativePath);

typedef struct
{
	fnptr_platform_fetch_res_file* FetchResourceFile;
	fnptr_platform_fetch_res_size* FetchResourceSize;
} res_handler_interface;

/** Engine -> Platform */
typedef i32 fnptr_engine_init(void* memStore, u64 memSize, window_props* windowProps, res_handler_interface* ResHandler);
typedef i32 fnptr_engine_reinit(void* memStore, u64 memSize, window_props* windowProps, res_handler_interface* ResHandler);
typedef i32 fnptr_engine_runtime(window_props* windowProps, action_interface* InputHandle);


#endif