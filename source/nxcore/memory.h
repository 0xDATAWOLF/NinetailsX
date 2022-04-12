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
 * Defines the simplest unit of allocation.
 */
typedef struct
{
	void* base;
	void* offset;
	size_t length;
	size_t commit;
} memarena_t;

inline memarena_t
CreateMemoryArena(void* base, size_t length)
{
	memarena_t _arena;
	_arena.base = base;
	_arena.offset = base;
	_arena.commit = 0;
	_arena.length = length;
	return _arena;
}

/**
 * Alias macros for PushSize for structs (types) and arrays.
 */
#define PushStruct(memarena, struct_type) (struct_type*)PushSize(memarena, sizeof(struct_type))
#define PushArray(memarena, array_type, array_count) (array_type*)PushSize(memarena, sizeof(array_type)*array_count)

/**
 * Pushes a given size to a memory arena.
 */
inline void*
PushSize(memarena_t* memoryArena, size_t bytes)
{

	assert((memoryArena->commit + bytes) < memoryArena->length); // Ensure we don't run over the arena's limit.

	void* _allocation = memoryArena->offset;
	memoryArena->commit += bytes;
	memoryArena->offset = (u8*)memoryArena->offset + bytes;

	return _allocation;

}

/**
 * Pops a given size to a memory arena. This does not clear to 0.
 */
inline void
Pop(memarena_t* memoryArena, size_t bytes)
{

	memoryArena->offset = (u8*)memoryArena->offset - bytes;
	memoryArena->commit -= bytes;
	//nx_memset(_collection_ptr, bytes); // Do we actually want to slow pop?
	
}

#endif