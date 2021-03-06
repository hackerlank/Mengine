#	pragma once

#	include	"vec3.h"

#	include "config.h"

namespace mt
{
	class simplex_solver
	{
	public:
		MENGINE_MATH_METHOD_INLINE simplex_solver();

	public:
		MENGINE_MATH_METHOD_INLINE void reset();
		MENGINE_MATH_METHOD_INLINE void addWPQ( const mt::vec3f & w, const mt::vec3f & p, const mt::vec3f & q  );
		MENGINE_MATH_METHOD_INLINE bool update( mt::vec3f & V );

	private:
		MENGINE_MATH_METHOD_INLINE void remove_1();
		MENGINE_MATH_METHOD_INLINE void remove_2();
		MENGINE_MATH_METHOD_INLINE bool solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f & V );
		MENGINE_MATH_METHOD_INLINE bool solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f AC, mt::vec3f & V );

	private:
		int size;
		float eps;

		mt::vec3f W[3];
		mt::vec3f P[3];
		mt::vec3f Q[3];
	};
};

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include	"simplex_inline.h"
#	endif
