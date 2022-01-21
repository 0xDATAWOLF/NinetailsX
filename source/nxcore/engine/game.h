#ifndef NINETAILS_GAME_H
#define NINETAILS_GAME_H
#include <nxcore/primitives.h>

typedef struct memory_layout
{
	void* Base;
	u64 Size;
} memory_layout;

typedef i32 fnptr_engine_runtime(memory_layout* MemoryLayout);

#endif
