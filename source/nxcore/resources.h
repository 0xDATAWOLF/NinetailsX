#ifndef NINETAILSX_RESOURCES_H
#define NINETAILSX_RESOURCES_H
#include <nxcore/helpers.h>
#include <nxcore/resource/dibitmap.h>

/**
 * Returns a casted pointer to a bitmap_section if the contents are valid.
 */
inline bitmap_section*
GetDIBitmapHeaders(void* resourceData)
{
	
	bitmap_section* _bitmap_section = (bitmap_section*)resourceData;
	return _bitmap_section;

}

#endif