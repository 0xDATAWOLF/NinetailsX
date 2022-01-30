#include <nxcore/engine.h>
#include <nxcore/memory.h>
#include <nxcore/math.h>

typedef struct engine_state
{
	btmonotonic_memory_arena EngineMemoryArena;
	b32 Initialized;

	i32 x, y;
	b32 mov_flip;

} engine_state;

/**
 * TODO:
 * 			I'd like to learn how to draw an arbitrary line based on two coordinates.
 * 			This is a good place to start as it is fast and somewhat simple to do.
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
	else if (y+RectHeight >= Renderer->Height)
	{
		// Now we know that the rect is out of bounds, but how out of bounds is it?
		i32 Overbounds = (y+RectHeight) - Renderer->Height;
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
	if (x+RectWidth >= Renderer->Width)
	{
		i32 Overbounds = (x+RectWidth) - Renderer->Width;
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
	u32* Loc = (u32*)Renderer->Image + (Renderer->Width*RectY) + RectX;
	for (i32 Row = 0; Row < RectHeight; ++Row)
	{
		/**
		 * The pixel pitch places the pointer at the start of each row. Since we advance
		 * by the width of the buffer, we guaranteed to hit the next starting position
		 * one row down.
		 */
		u32* Pitch = Loc + (Row*Renderer->Width);
		for (i32 Col = 0; Col < RectWidth; ++Col)
		{
			u32* Pixel = Pitch + Col;
			*Pixel++ = color;
		}
	}


}

extern "C" __declspec(dllexport) i32
EngineRuntime(memory_layout* MemoryLayout, renderer* Renderer, input* InputHandle)
{

	/**
	 * Performing necessary start-up initialization. We may want to pull this out to its
	 * own function rather than what we are doing here. For now, this works.
	 * 
	 * NOTE:
	 * 			Engine initialization gathers the state from the heap pointer provided by
	 * 			MemoryLayout--we are casting it to engine_state. Therefore, if we want to use
	 * 			the heap, we need to bump the heap pointer up by sizeof(engine_state) to get
	 * 			starting point of the heap memory and the subtract the size of the heap by
	 * 			the size of the engine_state to get the actual remaining size.
	 */
	engine_state* EngineState =	(engine_state*)MemoryLayout->Base;
	if (EngineState->Initialized == NULL)
	{
		EngineState->Initialized = true;

		/**
		 * We will need to set up the memory layout. Since the engine state shares to head of
		 * memory layout, we need to subtract that from the total size and offset the base pointer
		 * accordingly.
		 */
		void* EngineHeapBasePointer = (void*)((u8*)MemoryLayout->Base + sizeof(engine_state));
		u32 EngineHeapSize = (u32)(MemoryLayout->Size - sizeof(engine_state));
		EngineState->EngineMemoryArena = CreateBTMonotonicMemoryArena(EngineHeapBasePointer, EngineHeapSize);


		/**
		 * We are going to emulate the NES screen resolution, 256x240. To do this, we are going to
		 * resize the renderer to the appropriate resolution and then grab an area on the heap required
		 * to fill that buffer.
		 */
		Renderer->Width = 160;
		Renderer->Height = 144;

	}
	
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
		Renderer->Width*Renderer->Height*sizeof(u32));

	// We will now fill the screen with a debug color: red!
	for (u32 pIndex = 0; pIndex < (u32)Renderer->Width*(u32)Renderer->Height; ++pIndex)
	{
		u32* Pixel = (u32*)Renderer->Image + pIndex;
		*Pixel = 0x00FF0000;
	}

	/**
	 * In order to test the frameStep, we're going to update out x-pos by a fixed time to see
	 * a simple animation of movement.
	 */

	if (EngineState->x == -50) EngineState->mov_flip = 1;
	else if (EngineState->x == 120) EngineState->mov_flip = 0;
	if (EngineState->mov_flip == 1)
	{
		EngineState->x++;
	}
	else
	{
		EngineState->x--;
	}

	DrawRect(Renderer, EngineState->x, EngineState->y, 100, 100, 0x00FFFFFF);

	v2 myVector = {0};
	myVector.x = 10;
	myVector.y = 10;

	v3 fooVector = {0};
	fooVector.xy = myVector;
	fooVector.y = 20;
	fooVector.z = 10;

	v4 barVector = {0};
	barVector.xyz = fooVector;
	barVector.zw = myVector;
	barVector.w += 10.0f;

	/**
	 * NOTE:
	 * 			The return value of this function is reserved--that is, if the application should close
	 * 			based on the request of the engine--the value should be non-zero. The platform layer
	 * 			needs to respect this by closing the application!
	 */
	return(0);
}