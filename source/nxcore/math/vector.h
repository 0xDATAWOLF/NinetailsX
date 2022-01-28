#ifndef NINETAILSX_VECTOR_H
#define NINETAILSX_VECTOR_H
#include <nxcore/primitives.h>

/**
 * A 2-component vector type.
 */
typedef union v2
{

	struct
	{
		r32 x, y;
	};

	struct {
		r32 u, v;
	};

	struct
	{
		r32 width, height;
	};

	r32 e[2];

#ifdef __cplusplus
	/**
	 * Returns
	 */
	inline r32&
	operator[](const i32& index)
	{
		return e[index];
	}
#endif

} v2;

/**
 * A 3-component vector type.
 */
typedef union v3
{

	struct
	{
		r32 x, y, z;
	};

	struct
	{
		r32 u, v, w;
	};

	struct
	{
		r32 r, g, b;
	};

	struct
	{
		v2 xy;
		r32 __ignored_a;
	};

	struct
	{
		r32 __ignored_b;
		v2 yz;
	};

	struct
	{
		v2 uv;
		r32 __ignored_c;
	};

	struct
	{
		r32 __ignored_d;
		v2 vw;
	};

	r32 e[3];

#ifdef __cplusplus
	inline r32&
	operator[] (const i32& index)
	{
		return e[index];
	}
#endif;

} v3;

typedef union v4
{

	struct
	{
		union
		{
			v3 xyz;
			struct
			{
				r32 x,y,z;
			};
		};
		r32 w;
	};

	struct
	{
		union
		{
			v3 rgb;
			struct
			{
				r32 r, g, b;
			};
		};
		r32 a;
	};

	struct
	{
		v2 xy;
		r32 __ignored_a;
		r32 __ignored_b;
	};

	struct
	{
		r32 __ignored_c;
		v2 yz;
		r32 __ignored_d;
	};

	struct
	{
		r32 __ignored_e;
		r32 __ignored_f;
		v2 zw;
	};

	r32 e[4];

#ifdef __cplusplus
	inline r32&
	operator[](const i32& index)
	{
		return e[index];
	}
#endif

} v4;

#endif