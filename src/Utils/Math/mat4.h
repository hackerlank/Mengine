#	pragma once

#	include "config.h"

#	include "utils.h"

#	include "vec4.h"
#	include "mat3.h"

namespace mt
{
	struct mat4f
	{
		typedef vec3f value_type;
		typedef float element_type;

		vec4f v0;
		vec4f v1;
		vec4f v2;
		vec4f v3;

		MENGINE_MATH_METHOD_INLINE mat4f();
		MENGINE_MATH_METHOD_INLINE mat4f( const mat4f& _rhs );

		MENGINE_MATH_METHOD_INLINE vec4f& operator [] ( size_t i );
		MENGINE_MATH_METHOD_INLINE const vec4f& operator [] ( size_t i )const;

		MENGINE_MATH_METHOD_INLINE mat4f& operator = (const mat4f& _rhs);

		MENGINE_MATH_METHOD_INLINE float * buff();
		MENGINE_MATH_METHOD_INLINE const float * buff() const;
	};

	MENGINE_MATH_FUNCTION_INLINE bool operator==(const mat4f& _a, const mat4f& _b);
	MENGINE_MATH_FUNCTION_INLINE bool operator!=(const mat4f& _a, const mat4f& _b);

	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const mat4f& m, const vec3f& v);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const vec3f& v, const mat4f& m);

	MENGINE_MATH_FUNCTION_INLINE vec4f operator*(const mat4f& m, const vec4f& v);
	MENGINE_MATH_FUNCTION_INLINE vec4f operator*(const vec4f& v, const mat4f& m);

	MENGINE_MATH_FUNCTION_INLINE mat4f operator*(const mat4f& _a, const mat4f& _b);
	MENGINE_MATH_FUNCTION_INLINE mat4f operator*(const mat4f& _a, const mat3f& _b);

	MENGINE_MATH_FUNCTION_INLINE mat4f operator+(const mat4f& _a, const mat4f& _b);
	MENGINE_MATH_FUNCTION_INLINE mat4f operator-(const mat4f& _a, const mat4f& _b);

	MENGINE_MATH_FUNCTION_INLINE bool cmp_m4_m4( const mat4f& _a, const mat4f& _b );

	MENGINE_MATH_FUNCTION_INLINE void mul_m4_v3( vec3f& _out, const mat4f& _m, const vec3f& _v );

	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v3_m4( vec3f& _out, const vec3f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v3_m4_r( vec3f& _out, const vec3f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE float mul_v3_v3_m4_proj( vec3f& _out, const vec3f& _v, const mat4f& _m );

	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v2_m4( vec2f& _out, const vec2f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v2_m4_r( vec2f& _out, const vec2f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v3_m4( vec2f& _out, const vec3f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v3_m4_r( vec2f& _out, const vec3f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v2_m4( vec3f& _out, const vec2f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_m4_r( vec2f& _out, const vec2f& _v, const mat4f& _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v2_m4_r( vec3f& _out, const vec2f& _v, const mat4f& _m );

	MENGINE_MATH_FUNCTION_INLINE void mul_m4_v4( vec4f & _out, const mat4f & _m, const vec4f & _v );
	MENGINE_MATH_FUNCTION_INLINE void mul_v4_m4( vec4f & _out, const vec4f & _v, const mat4f & _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v4_v3_m4( vec4f & _out, const vec3f & _v, const mat4f & _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v3_m4_homogenize( vec3f & _out, const vec3f & _v, const mat4f & _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v3_m4_homogenize( vec2f & _out, const vec3f & _v, const mat4f & _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v2_m4_homogenize( vec3f & _out, const vec2f & _v, const mat4f & _m );
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v2_m4_homogenize( vec2f & _out, const vec2f & _v, const mat4f & _m );
	

	MENGINE_MATH_FUNCTION_INLINE void mul_m4_m4( mat4f& _out, const mat4f& _a, const mat4f& _b );

	MENGINE_MATH_FUNCTION_INLINE void mul_v4_m3_i( vec4f & _out, const vec4f & _a, const mat3f& _b );
	MENGINE_MATH_FUNCTION_INLINE void mul_m4_m3( mat4f& _out, const mat4f& _a, const mat3f& _b );

	MENGINE_MATH_FUNCTION_INLINE void add_m4_m4( mat4f& _out, const mat4f& _a, const mat4f& _b );
	MENGINE_MATH_FUNCTION_INLINE void sub_m4_m4( mat4f& _out, const mat4f& _a, const mat4f& _b );

	MENGINE_MATH_FUNCTION_INLINE void scale_m4_m4( mat4f& _out, const mat4f& _rhs, const mt::vec4f & _val );
	MENGINE_MATH_FUNCTION_INLINE void scale_m4( mat4f& _out, const mt::vec4f &_val );

	MENGINE_MATH_FUNCTION_INLINE void scale_rotate_m4_m4( mat4f& _out, const mat4f& _rhs, const mt::vec3f & _val );
	MENGINE_MATH_FUNCTION_INLINE void scale_rotate_m4( mat4f& _out, const mt::vec3f &_val );

	MENGINE_MATH_FUNCTION_INLINE void zero_m4( mat4f& _out );
	MENGINE_MATH_FUNCTION_INLINE void ident_m4( mat4f& _out );

	MENGINE_MATH_FUNCTION_INLINE bool is_ident_m34( const mat4f& _out );

	MENGINE_MATH_FUNCTION_INLINE void get_col_m4( vec4f& out, const mat4f& _rhs, int _index );
	MENGINE_MATH_FUNCTION_INLINE vec4f get_col_m4( const mat4f& _rhs, int _index );
	MENGINE_MATH_FUNCTION_INLINE void set_col_m4( mat4f& out, const vec4f& _rhs, int _index );

	MENGINE_MATH_FUNCTION_INLINE void transpose_m4( mat4f& _out, const mat4f& _rhs );
	MENGINE_MATH_FUNCTION_INLINE mat4f transpose_m4( const mat4f& _rhs );

	MENGINE_MATH_FUNCTION_INLINE void extract_m4_m3( mat3f& _out, const mat4f& _rhs );

	MENGINE_MATH_FUNCTION_INLINE void inv_m4_m4( mat4f& _out, const mat4f& _rhs );
	MENGINE_MATH_FUNCTION_INLINE mat4f inv_m4( const mat4f& _rhs );

	MENGINE_MATH_FUNCTION_INLINE void rotate_axis_m4( mat4f &out, const vec3f &u, float degrees );

	MENGINE_MATH_FUNCTION_INLINE void make_projection_fov_m4( mat4f &_out, float _fov, float _aspect, float _zn, float _zf );
	MENGINE_MATH_FUNCTION_INLINE void make_projection_fov2_m4( mat4f &_out, float _fov, float _aspect, float _zn, float _zf );
	MENGINE_MATH_FUNCTION_INLINE void make_projection_ortho_lh_m4( mat4f & _out, float _left, float _right, float _top, float _bottom, float _near, float _far );
	MENGINE_MATH_FUNCTION_INLINE void make_projection_frustum_m4( mat4f & _out, float _left, float _right, float _top, float _bottom, float _near, float _far );

	MENGINE_MATH_FUNCTION_INLINE void make_rotate_m4_euler( mat4f & _out, float _x, float _y, float _z );
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_m4_direction( mat4f & _out, const vec3f & _direction, const vec3f & _up );
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_m4_fixed_up( mat4f & _out, const vec3f & _direction, const vec3f & _up );
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_m4_fixed_left( mat4f & _out, const vec3f & _direction, const vec3f & _left );
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_m4_axes( mat4f & _out, const vec3f & _direction, const vec3f & _left, const vec3f & _up );


	MENGINE_MATH_FUNCTION_INLINE void make_rotate_x_axis_m4( mat4f & _out, float _angle );
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_y_axis_m4( mat4f & _out, float _angle );
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_z_axis_m4( mat4f & _out, float _angle );

	MENGINE_MATH_FUNCTION_INLINE void make_scale_m4( mat4f & _out, float _x, float _y, float _z );
	MENGINE_MATH_FUNCTION_INLINE void make_translation_m4_v3( mat4f & _out, const mt::vec3f & _pos );
	MENGINE_MATH_FUNCTION_INLINE void make_translation_m4( mat4f & _out, float _x, float _y, float _z );

	MENGINE_MATH_FUNCTION_INLINE void make_lookat_m4( mat4f & _out, const vec3f & _eye, const vec3f & _dir, const vec3f & _up, float _sign );

	MENGINE_MATH_FUNCTION_INLINE void project_m4( vec3f & _out, const vec3f & _vertex, float _width, float _height, const mat4f & _projection, const mat4f & _view, const mat4f & _world );

	MENGINE_MATH_FUNCTION_INLINE void make_euler_angles( vec3f & _euler, const mat4f & _rotate );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "mat4_inline.h"
#	endif
