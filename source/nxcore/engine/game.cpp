#include <nxcore/engine/game.h>
#include <nxcore/memory.h>

typedef struct engine_soft_renderer_state
{
	i32 Width;
	i32 Height;
	u32* Data;
} engine_soft_renderer;

typedef struct engine_state
{
	NXA::btmonotonic_memory_arena EngineMemoryArena;
	engine_soft_renderer_state RendererState;
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
		 * We will need to set up the memory layout. Since the engine state shares to head of
		 * memory layout, we need to subtract that from the total size and offset the base pointer
		 * accordingly.
		 */
		void* EngineHeapBasePointer = (void*)((u8*)MemoryLayout->Base + sizeof(engine_state));
		u32 EngineHeapSize = (u32)(MemoryLayout->Size - sizeof(engine_state));
		EngineState->EngineMemoryArena = NXA::CreateBTMonotonicMemoryArena(EngineHeapBasePointer, EngineHeapSize);


		/**
		 * We are going to emulate the NES screen resolution, 256x240. To do this, we are going to
		 * resize the renderer to the appropriate resolution and then grab an area on the heap required
		 * to fill that buffer.
		 */
		Renderer->Width = 256;
		Renderer->Height = 240;
		EngineState->RendererState.Width = Renderer->Width;
		EngineState->RendererState.Height = Renderer->Height;
		EngineState->RendererState.Data = (u32*)BTMonotonicArenaPushBottomSize(&EngineState->EngineMemoryArena, (256*240)*sizeof(u32));

	}
	
	Renderer->Image = (void*)EngineState->RendererState.Data;
	for (u32 pIndex = 0; pIndex < 256*240; ++pIndex)
	{
		u32* Pixel = (u32*)Renderer->Image + pIndex;
		*Pixel = 0x00FF0000;
	}

	return(0);
}