#ifndef NINETAILSX_RENDERER_H
#define NINETAILSX_RENDERER_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>

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

#endif