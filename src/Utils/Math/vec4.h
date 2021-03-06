#	pragma once

/*	
	Vector4 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, norm, homogenize.
*/

#	include "config.h"

#	include "vec3.h"

namespace mt
{
	struct vec4f
	{
		typedef float value_type;
		typedef float element_type;

		float x;
		float y;
		float z;
		float w;

		MENGINE_MATH_METHOD_INLINE vec4f();
		MENGINE_MATH_METHOD_INLINE vec4f(float _x, float _y, float _z, float _w);
		MENGINE_MATH_METHOD_INLINE vec4f(const vec2f& _v0, const vec2f& _v1);
		MENGINE_MATH_METHOD_INLINE vec4f(const vec4f& _v);
		MENGINE_MATH_METHOD_INLINE explicit vec4f(const vec3f& _v);
		MENGINE_MATH_METHOD_INLINE vec4f(const vec3f& _v, float _w);

		MENGINE_MATH_METHOD_INLINE vec4f & operator=(const vec4f& _rhs);

		MENGINE_MATH_METHOD_INLINE float &	operator [](size_t i);
		MENGINE_MATH_METHOD_INLINE const float operator [](size_t i) const;

		MENGINE_MATH_METHOD_INLINE vec4f & operator+=(const vec4f & _rhs);
		MENGINE_MATH_METHOD_INLINE vec4f & operator-=(const vec4f &	_rhs);
		MENGINE_MATH_METHOD_INLINE vec4f &	operator *= (const vec2f & _rhs);
		MENGINE_MATH_METHOD_INLINE vec4f &	operator /= (const vec2f & _rhs);
		MENGINE_MATH_METHOD_INLINE vec4f &	operator *= (const float _rhs);
		MENGINE_MATH_METHOD_INLINE vec4f &	operator /= (const float _rhs);
		

		MENGINE_MATH_METHOD_INLINE float sqrlength() const;
		MENGINE_MATH_METHOD_INLINE float length() const;

		MENGINE_MATH_METHOD_INLINE const vec3f & to_vec3f() const;

		MENGINE_MATH_METHOD_INLINE float * buff();
		MENGINE_MATH_METHOD_INLINE const float * buff() const;
	};

	MENGINE_MATH_FUNCTION_INLINE bool operator==(const vec4f& _a, const vec4f & _b);
	MENGINE_MATH_FUNCTION_INLINE bool operator!=(const vec4f& _a, const vec4f & _b);

	MENGINE_MATH_FUNCTION_INLINE vec4f operator+(const vec4f& _a, const vec4f & _b);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator-(const vec4f& _a, const vec4f & _b);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator*(const vec4f& _rhs, const vec4f & _val);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator/(const vec4f& _rhs, const vec4f & _val);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator*(const vec4f& _rhs, const vec2f & _val);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator/(const vec4f& _rhs, const vec2f & _val);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator*(const vec4f& _rhs, const float _val);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator/(const vec4f& _rhs, const float _val);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator-(vec4f _rhs);
	
	MENGINE_MATH_FUNCTION_INLINE void ident_v4( vec4f & _rhs );
	MENGINE_MATH_FUNCTION_INLINE float length_v4( vec4f _rhs );

	MENGINE_MATH_FUNCTION_INLINE void extract_v4_v3( vec3f & _out, const vec4f & _rhs );

	MENGINE_MATH_FUNCTION_INLINE void set_v4_v3( vec4f & _out, const vec3f & _rhs );

	MENGINE_MATH_FUNCTION_INLINE bool cmp_v4_v4( const vec4f& _a, const vec4f& _b );

	MENGINE_MATH_FUNCTION_INLINE void add_v4_v3(vec4f& _out, const vec4f& _a, const vec3f& _b);
	MENGINE_MATH_FUNCTION_INLINE void add_v4_v4(vec4f& _out, const vec4f& _a, const vec4f& _b);

	MENGINE_MATH_FUNCTION_INLINE void sub_v4_v4(vec4f& _out, const vec4f& _a, const vec4f& _b);
	MENGINE_MATH_FUNCTION_INLINE void scale_v4_v4(vec4f& _out, const vec4f& _a, float _val);
	MENGINE_MATH_FUNCTION_INLINE void neg_v4(vec4f& _out);
	
	MENGINE_MATH_FUNCTION_INLINE float dot_v4_v3(const vec4f &a, const vec3f &b);
	MENGINE_MATH_FUNCTION_INLINE float dot_v4_v4(const vec4f &a, const vec4f &b);
	
	MENGINE_MATH_FUNCTION_INLINE void norm_v4(vec4f& _out, const vec4f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE vec4f norm_v4(const vec4f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE void norm_safe_v4(vec4f& out, const vec4f& _rhs, float err = 1e-06);
	MENGINE_MATH_FUNCTION_INLINE vec4f norm_safe_v4(const vec4f& _rhs);
	
	MENGINE_MATH_FUNCTION_INLINE void homogenize_v4(vec4f& _out, const vec4f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE void homogenize_v3_v4(vec3f & _out, const vec4f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE vec4f homogenize_v4(const vec4f& _rhs);
};

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "vec4_inline.h"
#	endif
