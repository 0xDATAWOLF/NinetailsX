#ifndef NINETAILSX_RESOURCES_H
#define NINETAILSX_RESOURCES_H
#include <nxcore/helpers.h>
#include <nxcore/resource/dibitmap.h>

typedef u32 fnptr_platform_fetch_res_file(char* RelativePath, void* Buffer, u32 BuffSize);
typedef u32 fnptr_platform_fetch_res_size(char* RelativePath);

#endif