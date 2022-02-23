#include <nxcore/engine.h>

/**
 * Globals defines all engine globals.
 */
#include "globals.h"

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
	EngineState->testbitmap_res = BTMonotonicArenaPushBottomSize(&EngineState->EngineMemoryArena, BitmapFileSize);
	FetchResourceFile("./assets/test.bmp", EngineState->testbitmap_res, BitmapFileSize); // Fetches the file.

	EngineState->testbitmap = GetBitmapFromResource(EngineState->testbitmap_res);

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
	 * 			memory into it so long as we reset it each frame.
	 */
	ResetBTMonotonicMemoryArenaTop(&EngineState->EngineMemoryArena);

	/**
	 * We need to allocate space for the software bitmap. Since we know that the bitmap will be an
	 * exact fill for the window, we can simply calculate the size by multiplying out the width, height,
	 * and the bits-per-pixel (32bits, XRGB).
	 */
	//Renderer->Image = BTMonotonicArenaPushTopSize(&EngineState->EngineMemoryArena,
	//	Renderer->WindowDimensions.width*Renderer->WindowDimensions.height*sizeof(u32));
	
	// Actually, now we can just define a standard bitmap as our screen.
	EngineRenderer->screenBitmap = CreateBitmapLayer(&EngineState->EngineMemoryArena, Renderer->WindowDimensions);
	EngineRenderer->Image = EngineRenderer->screenBitmap.buffer; // Map Image to bitmap buffer location.

	/**
	 *	We are filling the background to clear out the contents of the last frame.
	 */
	//DrawRect(EngineRenderer, 0, 0, EngineRenderer->WindowDimensions.width,
	//	EngineRenderer->WindowDimensions.height, CreateDIBPixel(1.0f, 1.0f, 0.0f, 0.0f));
	DrawRect(&EngineRenderer->screenBitmap, {0,0}, EngineRenderer->WindowDimensions,
		CreateDIBPixel(1.0f, 1.0f, 0.0f, 0.0f));

	/**
	 * Drawing the test bitmap!
	 */
#if 1
	DrawBitmap(EngineRenderer, EngineState->testbitmap.buffer,
		0, 0,
		EngineState->testbitmap.dims.width, EngineState->testbitmap.dims.height);
#else
	texture testTex = CreateTextureFromBitmap(&EngineState->testbitmap, {16,16}, {16,16});
	DrawTexture(EngineRenderer, &testTex, -8, -8);
#endif

	/**
	 * NOTE:
	 * 			The return value of this function is reserved--that is, if the application should close
	 * 			based on the request of the engine--the value should be non-zero. The platform layer
	 * 			needs to respect this by closing the application!
	 */
	return(0);
}