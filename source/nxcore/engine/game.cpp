#include "game.h"
#include <windows.h>

typedef struct engine_state
{
	b32 Initialized;
} engine_state;

extern "C" __declspec(dllexport) i32
EngineRuntime(memory_layout* MemoryLayout)
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
	}

	

	return(0);
}