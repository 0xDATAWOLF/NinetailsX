#ifndef NINETAILSX_MEMORY_H
#define NINETAILSX_MEMORY_H
#include <nxcore/helpers.h>

/**
 * nx_memcopy
 * 		Copies memory from source to destination using a given byte count.
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
 * nx_memset
 * 			Sets a region of memory to a specified value.
 */

// Sets a region of memory with a given value.
internal void
__nx_memset_unaligned(void* Dest, u32 ByteCount, u8 Value)
{
	for (u32 bIndex = 0; bIndex < ByteCount; ++bIndex)
		*((u8*)Dest+bIndex) = Value;
}

// Sets a region of memory with a given value at 64-bit increments.
internal void
__nx_memset_aligned_64bit(void* Dest, u32 ByteCount, u64 Value)
{
#ifdef NINETAILSX_DEBUG
	assert((ByteCount % 8) == 0);
#endif
	for (u32 bIndex = 0; bIndex < (ByteCount*8)/64; ++bIndex)
		*((u64*)Dest+bIndex) = Value;
}

/**
 * Sets a region of memory to a specified value.
 * 
 * TODO:
 * 			Much like nx_memcopy, we can probably use SSE instructions to potentially
 * 			speed this up. There's a good possibility that this function will be called
 * 			often to reset certain regions of memory to zero every frame, so it may be
 * 			necessary to profile this and see how we can speed this up. For now,
 * 			this will work fine as a 64bit operation.
 * 
 * 			Additionally, some method of making this branchless would be ideal.
 */
internal void
nx_memset(void* Dest, u32 ByteCount, u8 Value = 0x00)
{

	u32 unaligned = ByteCount % 8;
	u32 aligned = ByteCount - unaligned;

	// No fancy bit manip required for zero.
	if (Value == 0x00)
	{
		__nx_memset_aligned_64bit(Dest, aligned, (u64)0x00);
	}

	// We need to perform some bit magic to spread this value across
	// a 64-bit value.
	else
	{
		u64 __aligned16 = (Value << 8) | Value;
		u64 __aligned32 = (__aligned16 << 16) | __aligned16;
		u64 __aligned64 = (__aligned32 << 32) | __aligned32;
		__nx_memset_aligned_64bit(Dest, aligned, __aligned64);
	}

	__nx_memset_unaligned((u8*)Dest+aligned, unaligned, Value);

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

/**
* monotonic_memory_arena
* 			This is a simple monotonic allocator which contains a fixed space,
* 			a basepointer at the start, and an offset pointer of the next free
* 			location. This allocator is quite simplistic, great for temporary
* 			storage of data that will be released in bulk.
*/
typedef struct monotonic_memory_arena
{
	void* Base; // Reference only.
	void* Offset;
	u64 Size;
	u64 Commit;
} monotonic_memory_arena;

/**
 * btmonotonic_memory_arena
 * 			This is a "bottom-top" monotonic memory allocator; essentially there are two
 * 			pointer locations for the bottom and the top. The bottom grows up, the top grows
 * 			down. The ideal use-case for this type of allocator is functionally similar to
 * 			a standard monotonic_memory_arena, except that it draws from the same heap range
 * 			but contains to separate areas to draw from. This schema works well for frame-allocators
 * 			which reset every frame and areas of heap which stay used for long periods of time.
 * 
 * 			Since this allocator is rather stupid, we are only concerned with the total commit.
 * 			Should any allocation every go over the size of the arena, we will know if bottom
 * 			or top pointers will collide. 
 */

typedef struct btmonotonic_memory_arena
{
	void* Base; // Reference only.
	void* Bottom;
	void* Top;
	void* OffsetBottom;
	void* OffsetTop;
	u64 Size;
	u64 Commit;
} btmonotonic_memory_arena;

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
 * Creates a btmonotonic_memory_allocator with a given pointer to a region in memory
 * and a given size.
 */
btmonotonic_memory_arena
CreateBTMonotonicMemoryArena(void* BasePointer, u64 Size)
{
	btmonotonic_memory_arena _Arena = {0};
	_Arena.Base = BasePointer;
	_Arena.OffsetBottom = BasePointer;
	_Arena.OffsetTop = (void*)((u8*)BasePointer+Size);
	_Arena.Size = Size;
	return _Arena;
}

/**
 * Resets a monotonic memory arena and sets all used bytes to 0.
 */
void
ResetMonotonicMemoryArena(monotonic_memory_arena* Arena)
{
	nx_memset(Arena->Base, (u32)Arena->Commit);
	Arena->Commit = 0;
	Arena->Offset = Arena->Base;
	return;
}

/**
 * Resets the bottom portion of the bottom-top monotonic memory
 * arena and sets all used bytes to 0.
 */
void
ResetBTMonotonicMemoryArenaBottom(btmonotonic_memory_arena* Arena)
{
	// We need to find out how much space the bottom occupies, we can do
	// this by subtracting the base from the bottom offset which gives us
	// the amount of room the allocation has used.
	u32 BottomSize = (u32)((u8*)Arena->OffsetBottom - (u8*)Arena->Base);
	nx_memset(Arena->Base, BottomSize);

	// Since the commit is shared with the top as well, we just subtract the bottom portion.
	Arena->Commit -= BottomSize;
	Arena->OffsetBottom = Arena->Base;
}

/**
 * Resets the top portion of the bottom-top monotonic memory
 * arena and sets all used bytes to 0.
 */
void
ResetBTMonotonicMemoryArenaTop(btmonotonic_memory_arena* Arena)
{
	// We need to find out how much space the top occupies similar to
	// to the way we did it for resetting the bottom, except now
	// we are subtracting to offset from the end of the allocation range.
	u32 TopSize = (u32)(((u8*)Arena->Base + Arena->Size) - ((u8*)Arena->OffsetTop));

	nx_memset(Arena->OffsetTop, TopSize);
	Arena->Commit -= TopSize;
	Arena->OffsetTop = (void*)((u8*)Arena->Base + Arena->Size);

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
	// TODO:	We need a fail-safe mechanism to handle conditions which the
	// 			allocator can not accomodate a push.
#ifdef NINETAILSX_DEBUG
	assert(Arena->Commit + Bytes < Arena->Size);
#endif

	void* _MemoryPointer = Arena->Offset;
	Arena->Commit += Bytes;
	Arena->Offset = (void*)((u8*)Arena->Offset + Bytes);
	return(_MemoryPointer);

}

/**
 * Various helpers for the bottom-top memory allocator.
 */
#define BTMonotonicArenaPushBottomSize(ArenaPtr, Size) (void*)__push_bottom_size(ArenaPtr, Size)
#define BTMonotonicArenaPushBottomStruct(ArenaPtr, StructType) (StructType*)__push_bottom_size(ArenaPtr, sizeof(StructType))
#define BTMonotonicArenaPushBottomArray(ArenaPtr, ArrayType, ArrayCount) (ArrayType*)__push_bottom_size(ArenaPtr, sizeof(ArrayType)*ArrayCount)
#define BTMonotonicArenaPushTopSize(ArenaPtr, Size) (void*)__push_top_size(ArenaPtr, Size)
#define BTMonotonicArenaPushTopStruct(ArenaPtr, StructType) (StructType*)__push_top_size(ArenaPtr, sizeof(StructType))
#define BTMonotonicArenaPushTopArray(ArenaPtr, ArrayType, ArrayCount) (ArrayType*)__push_top_size(ArenaPtr, sizeof(ArrayType)*ArrayCount)

/**
 * Returns a pointer to a region of free memory from the bottom of the arena.
 */
void*
__push_bottom_size(btmonotonic_memory_arena* Arena,  u64 Bytes)
{
	// TODO:	We need a fail-safe mechanism to handle conditions which the
	// 			allocator can not accomodate a push.
#ifdef NINETAILSX_DEBUG
	assert(Arena->Commit + Bytes < Arena->Size);
#endif
	void* _MemoryPointer = Arena->OffsetBottom;
	Arena->Commit += Bytes;
	Arena->OffsetBottom = (void*)((u8*)Arena->OffsetBottom + Bytes);
	return(_MemoryPointer);
}

/**
 * Returns a pointer to a region of free memory from the top of the arena.
 */
void*
__push_top_size(btmonotonic_memory_arena* Arena, u64 Bytes)
{
	// TODO:	We need a fail-safe mechanism to handle conditions which the
	// 			allocator can not accomodate a push.
#ifdef NINETAILSX_DEBUG
	assert(Arena->Commit + Bytes < Arena->Size);
#endif
	// NOTE:	We are growing downwards from the top of allocation.
	void* _MemoryPointer = (u8*)Arena->OffsetTop - Bytes;
	Arena->Commit += Bytes;
	Arena->OffsetTop = _MemoryPointer;
	return(_MemoryPointer);

}

#endif