/**
 * 
 * The TODO-List
 * 
 * 1. Bitmap Renderer Model
 * 		a. Bitmap Layers (Base Layer, composit layers... z-ordering?)
 * 		b. Textures (Image loading, make textures, resource handling)
 * 		c. Rectangle Draw (Simple primitive drawing)
 * 		d. Objects & Models (Contains Texture + UV)
 * 		e. Combine Layers (May need performance modification)
 * 		f. Final bitmap to Platform
 * 
 * 2. Application Scaling
 * 		a. Set up fixed resolutions.
 * 		b. Letter boxing, centering + offset controls
 * 		c. Set up dynamic resolutions (to the nearested power of 2 scaling)
 * 
 * 3. Cleanup & Refactoring
 * 		a. Globals (Platform-to-Engine Functions, Engine State)
 * 		b. Untie Struct Dependencies in Engine.h
 */

#include <nxcore/engine.h>
#include <nxcore/core.h>

global engine_state* EngineState;
fnptr_platform_fetch_res_file* FetchResourceFile;
fnptr_platform_fetch_res_size* FetchResourceSize;

typedef struct
{
	texture_t tex;
	v3 position;
	v2 UV;
} entity_t;

/**
 * The re-initialization method if the engine DLL is reloaded during run-time.
 * Perform necessary re-initialization here.
 * 
 * NOTE:
 * 			DLL reloading has not implemented, so this shouldn't really be
 * 			used for anything yet.
 */
NinetailsXAPI i32
EngineReinit(void* memStore, u64 memSize, window_props* window_props, res_handler_interface* ResourceHandler)
{

	// Initialized resource handler functions as well.
	FetchResourceFile = ResourceHandler->FetchResourceFile;
	FetchResourceSize = ResourceHandler->FetchResourceSize;

	// Cast the MemoryLayout to engine_state which contains the engine's persistent state.
	// When the DLL reloads, this is how we persist the state.
	EngineState = (engine_state*)memStore;

	return 0;	
}


/**
 * Engine initialization which occurs before the runtime of the engine. This handles
 * all internal memory formatting, necessary resource loading, and other critical engine
 * component initialization.
 */
NinetailsXAPI i32
EngineInit(void* memStore, u64 memSize, window_props* windowProps, res_handler_interface* ResourceHandler)
{

	// We call EngineReinit here because it will set all the engine globals to the correct state.
	EngineReinit(memStore, memSize, windowProps, ResourceHandler);

	// Initialize window dimensions on start up.
	windowProps->dimensions = { 160, 144 };

	/**
	 * We need to initialize the engine_state since EngineInit is called before the runtime performs
	 * any actions. EngineReinit will reliably be called at any time during the runtime, so we need
	 * to ensure that everything is correctly initialized here such that the state persists.
	 */
	EngineState->Initialized = true;
	void* EngineHeapBasePointer = (void*)((u8*)memStore + sizeof(engine_state));
	u64 EngineHeapSize = (u64)(memSize - sizeof(engine_state));
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
EngineRuntime(window_props* windowProps, action_interface* InputHandle)
{

	/**
	 * NOTE:
	 * 			We are using the top of our memory allocator as our frame allocator.
	 * 			This will reset every frame, therefore it's safe to shove volatile
	 * 			memory into it so long as we reset it each frame.
	 */
	ResetBTMonotonicMemoryArenaTop(&EngineState->EngineMemoryArena);

	/**
	 * We need to allocate space for the software bitmap and then set the window properties source buffer
	 * to that bitmap so it can be drawn.
	 */
	EngineState->base_layer = CreateBitmapLayer(&EngineState->EngineMemoryArena, windowProps->dimensions);
	windowProps->softwareBitmap = EngineState->base_layer.buffer;

	/**
	 * We are filling the background to clear out the contents of the last frame then we are drawing a
	 * bitmap to test the basic drawing functions.
	 */
	DrawRect(&EngineState->base_layer, {0,0}, EngineState->base_layer.dims,
		CreateDIBPixel(1.0f, 1.0f, 0.0f, 0.0f));
	DrawBitmap(&EngineState->base_layer, &EngineState->testbitmap, {80,80});

	/**
	 * NOTE:
	 * 			The return value of this function is reserved--that is, if the application should close
	 * 			based on the request of the engine--the value should be non-zero. The platform layer
	 * 			needs to respect this by closing the application!
	 */
	return(0);
}