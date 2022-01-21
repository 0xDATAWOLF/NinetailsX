#ifndef NINETAILSX_MEMORY_H
#define NINETAILSX_MEMORY_H
#include <nxcore/helpers.h>

/**
 * 
 * NX_MEMCOPY
 * 
 * 		Copies memory from source to destination using a given byte count.
 * 
 */

// Copies unaligned in single-byte increments.
inline void
__nx_memcopy_unaligned(void* Dest, void* Source, u32 ByteCount)
{
	for (u32 bIndex = 0; bIndex < ByteCount; ++bIndex)
		*((u8*)Dest+bIndex) = *((u8*)Source+bIndex);
}

// Copies 64-bit aligned in eight-byte increments. 
inline void
__nx_memcopy_aligned_64bit(void* Dest, void* Source, u32 ByteCount)
{
#ifdef NINETAILSX_DEBUG
	// Enforce that the byte count is perfectly aligned. If its not,
	// this may accidently copy data out of bounds!
	assert((ByteCount % 8) == 0);
#endif
	for (u32 bIndex = 0; bIndex < (ByteCount*8)/64; ++bIndex)
		*((u64*)Dest+bIndex) = *((u64*)Source+bIndex);
}

/**
 * Copies memory to a given destination.
 * 
 * TODO:
 * 			We can use SSE instructions to potentially speed this up
 * 			so long as it aligns on 128bit/256bit boundaries. We will
 * 			need to dynamically determine CPU compatability for this.
 */
internal void
nx_memcopy(void* Dest, void* Source, u32 ByteCount)
{

	u32 unaligned = ByteCount % 8;
	u32 aligned = ByteCount - unaligned;
	__nx_memcopy_aligned_64bit(Dest, Source, aligned);
	__nx_memcopy_unaligned((u8*)Dest+aligned, (u8*)Source+aligned, unaligned);

}

/**
 * NXA - NinetailsX Allocators
 * 			This is where the engine's various memory allocators are defined.
 * 
 * TODO:
 * 			memory_pool - Create a paging pool for dynamic memory allocations.
 * 			We can use this pool for smaller allocations which we may need to do
 * 			for more advanced data structures such as dynamic linked lists, vectors,
 * 			etc.
 * 
 * TODO:
 * 			We need a fail-safe mechanism if an allocation is larger than the
 * 			allocator can fit--that is, for now, we fail fast with assertions,
 * 			but we will need to eventually need to consider hardware constraints.
 *  
 */

namespace NXA
{

	/*
	* monotonic_memory_arena
	* 			This is a simple monotonic allocator which contains a fixed space,
	* 			a basepointer at the start, and an offset pointer of the next free
	* 			location. This allocator is quite simplistic, great for temporary
	* 			storage of data that will be released in bulk.
	*/
	typedef struct monotonic_memory_arena
	{
		void* Base;
		void* Offset;
		u64 Size;
		u64 Commit;
	} monotonic_memory_arena;

	/**
	 * Creates a monotonic_memory_arena with a given pointer to a region in memory
	 * and a given size.
	 */
	monotonic_memory_arena
	CreateMonotonicMemoryArena(void* BasePointer, u64 Size)
	{
		monotonic_memory_arena _Arena = {0};
		_Arena.Base = BasePointer;
		_Arena.Offset = BasePointer;
		_Arena.Size = Size;
		return(_Arena);
	}

	/**
	 * Various push helpers for the monotonic arena allocator.
	 */
#define MonotonicArenaPushSize(ArenaPtr, Size) (void*)__push_size(ArenaPtr, Size)
#define MonotonicArenaPushStruct(ArenaPtr, StructType) (StructType*)__push_size(ArenaPtr, sizeof(StructType))
#define MonotonicArenaPushArray(ArenaPtr, ArrayType, ArrayCount) (ArrayType*)__push_size(ArenaPtr, sizeof(ArrayType)*ArrayCount)

	/**
	 * Returns a pointer to a region of free memory.
	 */
	void*
	__push_size(monotonic_memory_arena* Arena, u64 Bytes)
	{
#ifdef NINETAILSX_DEBUG
		// TODO:	We need a fail-safe mechanism to handle conditions which the
		// 			allocator can not accomodate a push.
		assert(Arena->Commit + Bytes < Arena->Size);
#endif

		void* _MemoryPointer = Arena->Offset;
		Arena->Commit += Bytes;
		Arena->Offset = (void*)((u8*)Arena->Offset + Bytes);
		return(_MemoryPointer);

	}

}

#endif