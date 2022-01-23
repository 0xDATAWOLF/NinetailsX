#include <nxcore/engine/game.h>
#include <nxcore/memory.h>

typedef struct engine_state
{
	NXA::btmonotonic_memory_arena EngineMemoryArena;
	b32 Initialized;
} engine_state;

extern "C" __declspec(dllexport) i32
EngineRuntime(memory_layout* MemoryLayout, renderer* Renderer)
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
		 * We will need to set up the memory layout.
		 */



		/**
		 * We are going to emulate the NES screen resolution, 256x240. To do this, we are going to
		 * resize the renderer.
		 */

		Renderer->Width = 256;
		Renderer->Height = 240;

	}

	

	return(0);
}