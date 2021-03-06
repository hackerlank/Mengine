#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class VectorVertices
	{
	public:
		VectorVertices();

	protected:
		typedef stdex::vector<RenderVertex2D> TVectorVertex2D;

	public:
		inline TVectorVertex2D & getVertices();
		inline void invalidateVertices( unsigned char _invalidate = 0xFE );

	protected:
		void updateVertices_();
	
	protected:
		virtual void _updateVertices( TVectorVertex2D & _vertices, unsigned char _invalidate ) = 0;

	private:
		TVectorVertex2D m_vertices;
		unsigned char m_invalidateVertices;
	};
	//////////////////////////////////////////////////////////////////////////
	inline VectorVertices::TVectorVertex2D & VectorVertices::getVertices()
	{
		if( m_invalidateVertices != 0 )
		{
			_updateVertices( m_vertices, m_invalidateVertices );
		}

		return m_vertices;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void VectorVertices::invalidateVertices( unsigned char _invalidate )
	{
		m_invalidateVertices |= _invalidate;
	}
}
