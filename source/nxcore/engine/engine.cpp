#include <nxcore/engine.h>
#include <nxcore/math.h>
#include <nxcore/resources.h>
#include <nxcore/renderer/colors.h>

/**
 * Globals defines all engine globals.
 */
#include "globals.h"

/**
 * TODO:
 * 			I'd like to learn how to draw an arbitrary line based on two coordinates.
 * 			This is a good place to start as it is fast and somewhat simple to do:
 * 	
 * Bresenham's Line Algorithm
 * 			https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
internal void
__DrawLine() { /** IMPLEMENT LATER */ return; };

/**
 * DrawRect
 * 			Draws a rectangle of a given size at a given position using a given hex-encoded color.
 * 			This color follows the the 0xXXRRGGBB format.
 */
internal void
DrawRect(renderer* Renderer, i32 x, i32 y, i32 width, i32 height, u32 color)
{

	// This is meant to preserve the state of the input variables for debugging purposes.
	i32 RectX = x;
	i32 RectY = y;
	i32 RectWidth = width;
	i32 RectHeight = height;

	/**
	 * Okay, so drawing rectangles isn't as easy as it seems. First, we are confined to the image
	 * buffer provided by the renderer. Since the x,y coordinates of the rectangle we are drawing
	 * can be any arbitrary value (and potentially be outside the range of the image buffer), we need
	 * to consider "out of bounds" drawing. First, let's look at the y-position.
	 * 
	 * Since the top-left corner is considered the origin, if the y-position is negative and we add
	 * the height, we will "know" if it is within bounds if the value is positive, negative if not.
	 * We don't need to draw anything if it is negative so we can just return.
	 */

	if (y+RectHeight < 0) return;

	// If y is negative, but still within bounds, then we can just shrink the height
	// by the y offset to adjust height such that it is within bounds and then set the y to 0.
	else if (y < 0)
	{
		RectHeight -= absolute_i32(y); // Value is negative, but for the sake understandability...
		RectY = 0;
	}

	// What about over-bounds y? Well, same deal. First, we see if the y+height is large than
	// the height of the bitmap region then we shrink it to fit.
	else if (y+RectHeight >= Renderer->WindowDimensions.height)
	{
		// Now we know that the rect is out of bounds, but how out of bounds is it?
		i32 Overbounds = (y+RectHeight) - Renderer->WindowDimensions.height;
		if (Overbounds >= RectWidth) return; // No point in drawing if it's completely out of bounds!

		// Now shrink the rectange by the overbounds.
		RectHeight -= Overbounds;
	}

	/**
	 * Now comes the question of the the x-bounds. Interestingly, if this isn't handled properly,
	 * it won't throw any errors but it will draw weird. The solution is the movement of the x-coord
	 * in the case that x is out of bounds left (but still drawable) and shrinking, and simply shrink
	 * if out of bounds on the right.
	 */
	if (x+RectWidth < 0) return; // Once again, if it's not in bounds, no need to draw.

	else if (x < 0)
	{
		RectWidth -= absolute_i32(x);
		RectX = 0;
	}

	/**
	 * In the case that the rect is out of the right bounds, we can just shrink the x.
	 */
	if (x+RectWidth >= Renderer->WindowDimensions.width)
	{
		i32 Overbounds = (x+RectWidth) - Renderer->WindowDimensions.width;
		if (Overbounds >= RectWidth) return; // If it's not even within bounds, exit.

		// Shrink.
		RectWidth -= Overbounds;
	}

	/**
	 * We need to grab the starting point of the draw location. First, we need to
	 * navigate down y-rows by incremementing the image base pointer over y*width.
	 * This will place us on the correct row. Then we just need to navigate to the right
	 * based on the x location and that will place at the exact starting location.
	 */
	u32* Loc = (u32*)Renderer->Image + (Renderer->WindowDimensions.width*RectY) + RectX;
	for (i32 Row = 0; Row < RectHeight; ++Row)
	{
		/**
		 * The pixel pitch places the pointer at the start of each row. Since we advance
		 * by the width of the buffer, we guaranteed to hit the next starting position
		 * one row down.
		 */
		u32* Pitch = Loc + (Row*Renderer->WindowDimensions.width);
		for (i32 Col = 0; Col < RectWidth; ++Col)
		{
			u32* Pixel = Pitch + Col;
			*Pixel++ = color;
		}
	}

}

/**
 * Determines if one bitmap exists within the boundary of another bitmap with the origin
 * placed at the left-lower corner.
 */
inline b32
IsWithinBitmapBounds(i32 parentWidth, i32 parentHeight, i32 x, i32 y, i32 subWidth, i32 subHeight)
{
	b32 _within_bounds = true;

	if (x+subWidth < 0) _within_bounds = false;
	if (x >= parentWidth) _within_bounds = false;
	if (y+subHeight < 0) _within_bounds = false;
	if (y >= parentHeight) _within_bounds = false;

	return _within_bounds;
}

/**
 * Draws a bitmap to the screen at a given position. The bitmapWidth and bitmapHeight *must*
 * be the exact size of the bitmap as it is required for proper pitch calculations.
 */
internal void
DrawBitmap(renderer* Renderer, void* bitmap, i32 x, i32 y, i32 bitmapWidth, i32 bitmapHeight)
{

	// Check within bounds, exit if it isn't.
	if (!IsWithinBitmapBounds(Renderer->WindowDimensions.width, Renderer->WindowDimensions.height,
		x, y, bitmapWidth, bitmapHeight)) return;

	// Set up the dest and source bitmap pointers.
	u32* destBitmap = (u32*)Renderer->Image + ((Renderer->WindowDimensions.width*y) + x);
	u32* sourceBitmap = (u32*)bitmap;

	// We need to preserve parameter values.
	i32 sourceWidth = bitmapWidth;
	i32 sourceHeight = bitmapHeight;
	
	// If we are slightly out of bounds, we need to clip the source and offset the source bitmap.
	if (x < 0)
	{
		i32 offsetX = absolute_i32(x);
		sourceWidth -= offsetX;
		sourceBitmap += offsetX; // Offsetting forward
		x = 0; // Placing back in bounds
	}

	// We do not need to offset the source bitmap, but we do need to shrink the width to fit
	// against the right edge.
	else if (x+sourceWidth >= Renderer->WindowDimensions.width)
	{
		sourceWidth -= ((x+sourceWidth) - Renderer->WindowDimensions.width);
	}

	// Applying what we did to the x-coordinates with they y-coordinates above, except now we need
	// to jump down rows.
	if (y < 0)
	{
		i32 offsetY = absolute_i32(y);
		sourceHeight -= offsetY;
		sourceBitmap += (bitmapWidth * offsetY); // Offsetting down y-rows
		y = 0; // Placing back in bounds
	}

	// Shrink to fit at y.
	else if (y+sourceHeight >= Renderer->WindowDimensions.height)
	{
		sourceHeight -= ((y+sourceHeight) - Renderer->WindowDimensions.height);
	}

	// Now we can copy to the buffer.
	for (i32 row = 0; row <= bitmapHeight; ++row)
	{
		u32* destPitch = destBitmap + (row*Renderer->WindowDimensions.width);
		u32* sourcePitch = sourceBitmap + (row*bitmapWidth);
		for (i32 col = 0; col < sourceWidth; ++col)
		{
			*destPitch++ = *sourcePitch++;
		}
	}

}

/**
 * The re-initialization method if the engine DLL is reloaded during run-time.
 * Perform necessary re-initialization here.
 * 
 * NOTE:
 * 			DLL reloading has not implemented, so this shouldn't really be
 * 			used for anything yet.
 */
NinetailsXAPI i32
EngineReinit(memory_layout* MemoryLayout, renderer* Renderer, res_handler_interface* ResourceHandler)
{

	// Store the renderer and memory_layout into globals.
	EngineMemoryLayout = MemoryLayout;
	EngineRenderer = Renderer;

	// Initialized resource handler functions as well.
	FetchResourceFile = ResourceHandler->FetchResourceFile;
	FetchResourceSize = ResourceHandler->FetchResourceSize;

	// Cast the MemoryLayout to engine_state which contains the engine's persistent state.
	// When the DLL reloads, this is how we persist the state.
	EngineState = (engine_state*)MemoryLayout->Base;

	return 0;	
}


/**
 * Engine initialization which occurs before the runtime of the engine. This handles
 * all internal memory formatting, necessary resource loading, and other critical engine
 * component initialization.
 */
NinetailsXAPI i32
EngineInit(memory_layout* MemoryLayout, renderer* Renderer, res_handler_interface* ResourceHandler)
{

	// We call EngineReinit here because it will set all the engine globals to the correct state.
	EngineReinit(MemoryLayout, Renderer, ResourceHandler);

	// Initialize window dimensions on start up.
	EngineRenderer->WindowDimensions = { 160, 144 };

	/**
	 * We need to initialize the engine_state since EngineInit is called before the runtime performs
	 * any actions. EngineReinit will reliably be called at any time during the runtime, so we need
	 * to ensure that everything is correctly initialized here such that the state persists.
	 */
	EngineState->Initialized = true;
	void* EngineHeapBasePointer = (void*)((u8*)MemoryLayout->Base + sizeof(engine_state));
	u32 EngineHeapSize = (u32)(MemoryLayout->Size - sizeof(engine_state));
	EngineState->EngineMemoryArena = CreateBTMonotonicMemoryArena(EngineHeapBasePointer, EngineHeapSize);

	/**
	 * Here, we are testing the resource fetching functions and bitmap stuff.
	 */
	u32 BitmapFileSize = FetchResourceSize("./assets/test.bmp");
	void* BitmapBuffer = BTMonotonicArenaPushTopSize(&EngineState->EngineMemoryArena, BitmapFileSize);
	FetchResourceFile("./assets/test.bmp", BitmapBuffer, BitmapFileSize); // Fetches the file.

	bitmap_section* BitmapSection = GetDIBitmapHeaders(BitmapBuffer); // Properly loads with 32-bit color index, DIB-V5.

	return 0;
}

/**
 * The frame-runtime of the engine.
 */
NinetailsXAPI i32
EngineRuntime(memory_layout* MemoryLayout, renderer* Renderer, action_interface* InputHandle)
{

	/**
	 * NOTE:
	 * 			We are using the top of our memory allocator as our frame allocator.
	 * 			This will reset every frame, therefore it's safe to shove volatile
	 * 			memory into it so long as we reset it.
	 */
	ResetBTMonotonicMemoryArenaTop(&EngineState->EngineMemoryArena);

	/**
	 * We need to allocate space for the software bitmap. Since we know that the bitmap will be an
	 * exact fill for the window, we can simply calculate the size by multiplying out the width, height,
	 * and the bits-per-pixel (32bits, XRGB).
	 */
	Renderer->Image = BTMonotonicArenaPushTopSize(&EngineState->EngineMemoryArena,
		Renderer->WindowDimensions.width*Renderer->WindowDimensions.height*sizeof(u32));

	/**
	 *	We are filling the background to clear out the contents of the last frame.
	 */
	DrawRect(EngineRenderer, 0, 0, EngineRenderer->WindowDimensions.width,
		EngineRenderer->WindowDimensions.height, CreateDIBPixel(1.0f, 1.0f, 0.0f, 0.0f));


	/**
	 * NOTE:
	 * 			The return value of this function is reserved--that is, if the application should close
	 * 			based on the request of the engine--the value should be non-zero. The platform layer
	 * 			needs to respect this by closing the application!
	 */
	return(0);
}