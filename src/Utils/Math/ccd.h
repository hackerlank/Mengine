#	pragma once

#	include "config.h"

#	include "vec3.h"
#	include "plane.h"

namespace mt
{	
	MENGINE_MATH_FUNCTION_INLINE bool ccd_sphere_sphere( const mt::vec3f & _center1, float _radius1, const mt::vec3f & _velocity1, const mt::vec3f & _center2, float _radius2, const mt::vec3f & _velocity2, float & _time, mt::vec3f & _normal );
	MENGINE_MATH_FUNCTION_INLINE bool ccd_ray_plane( const mt::vec3f & _point, const mt::vec3f & _velocity, const mt::planef & _plane, float & _time );
	MENGINE_MATH_FUNCTION_INLINE bool ccd_sphere_plane( const mt::vec3f & _center, float _radius, const mt::vec3f & _velocity, const mt::planef & _plane, float & _time );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#		include "ccd_inline.h"
#	endif
