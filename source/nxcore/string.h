#ifndef NINETAILSX_STRING_H
#define NINETAILSX_STRING_H
#include <nxcore/helpers.h>
#include <nxcore/memory.h>

/**
 * Finds the actual length of a string using the "safe" method. This function
 * expects that the string it is given is properly null-terminated. If it isn't, it
 * will return 0.
 */
internal u32
StringLength_s(char* string, u32 buff_size)
{

	u32 _size = 0;
	b32 null_terminated = 0;
	while (_size < buff_size) 
	{
		// Break out of the loop if we find the null-terminated.
		if (*(string + _size) == '\0')
		{
			null_terminated = 1;
			break;
		}
		_size++;
	}

	if (null_terminated == 0) return 0;
	else return _size;

}

/**
 * Concatenates string a and string b together and copies into dest buffer using the
 * "safe" method.
 */
internal void
ConcatenateStrings_s(char* a, u32 a_length, char* b, u32 b_length, char* dest, u32 dest_length)
{

	u32 _a_size_proper = StringLength_s(a, a_length);
	u32 _b_size_proper = StringLength_s(b, b_length);

#ifdef NINETAILSX_DEBUG
	assert(_a_size_proper > 0);
	assert(_b_size_proper > 0);
	assert(_a_size_proper + _b_size_proper < dest_length);
#endif

	// Copy everything over to the dest buffer.
	nx_memcopy(dest, a, _a_size_proper);
	nx_memcopy(dest+_a_size_proper, b, _b_size_proper);
	
	// Null terminate the string.
	*(dest+_a_size_proper+_b_size_proper) = '\0';

}

#endif