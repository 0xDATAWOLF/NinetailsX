#ifndef NINETAILSX_VECTOR_H
#define NINETAILSX_VECTOR_H
#include <nxcore/primitives.h>

/*********************************************************************************
 * 
 * Two Component Floating Point Vector
 * 
 ********************************************************************************/

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
	inline r32&
	operator[](const i32& index)
	{
		return e[index];
	}
#endif

} v2;

/*********************************************************************************
 * 
 * Three-Component Floating Point Vector
 * 
 ********************************************************************************/

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

/*********************************************************************************
 * 
 * Four-Component Floating Point Vector
 * 
 ********************************************************************************/

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







/*********************************************************************************
 * 
 * Two-Component Unsigned Integer Vector
 * 
 ********************************************************************************/

typedef union v2u
{
	struct 
	{
		u32 x, y;
	};
	struct
	{
		u32 width, height;
	};
	u32 e[2];
#ifdef __cplusplus
	inline u32&
	operator[](const i32& index)
	{
		return e[index];
	}
#endif
} v2u;

#ifdef __cplusplus
inline bool
operator==(const v2u& lhs, const v2u& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y) return true;
	else return false;
}

inline bool
operator!=(const v2u& lhs, const v2u& rhs)
{
	return !(lhs == rhs);
}

inline v2u
operator*(const v2u& lhs, const i32& rhs)
{
	return { lhs.x * rhs, lhs.y * rhs };
}

inline v2u
operator*(const i32& lhs, const v2u& rhs)
{
	return { rhs.x * lhs, rhs.y * lhs };
}

#endif



/*********************************************************************************
 * 
 * Two-Component Signed Integer Vector
 * 
 ********************************************************************************/

typedef union v2i
{
	struct 
	{
		i32 x, y;
	};
	struct
	{
		i32 width, height;
	};
	i32 e[2];
#ifdef __cplusplus
	inline i32&
	operator[](const i32& index)
	{
		return e[index];
	}
	inline v2i&
	operator*=(const i32 rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}
#endif
} v2i;

#ifdef __cplusplus
inline bool
operator==(const v2i& lhs, const v2i& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y) return true;
	else return false;
}

inline bool
operator!=(const v2i& lhs, const v2i& rhs)
{
	return !(lhs == rhs);
}

inline v2i
operator*(const v2i& lhs, const i32& rhs)
{
	return { lhs.x * rhs, lhs.y * rhs };
}

inline v2i
operator*(const i32& lhs, const v2i& rhs)
{
	return { rhs.x * lhs, rhs.y * lhs };
}
#endif

#endif