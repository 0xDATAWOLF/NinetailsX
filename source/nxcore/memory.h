#ifndef NINETAILSX_MEMORY_H
#define NINETAILSX_MEMORY_H
#include <nxcore/helpers.h>

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


#endif