#   include "Shape.h"

#   include "Kernel/Surface.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Shape::Shape()
		: m_invalidateVerticesLocal( true )
		, m_invalidateVerticesWM( true )
		, m_invalidateVerticesColor( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Shape::~Shape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setSurface( const SurfacePtr & _surface )
    {
        if( m_surface == _surface )
        {
            return;
        }

        m_surface = _surface;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const SurfacePtr & Shape::getSurface() const
    {
        return m_surface;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::_compile()
    {
        if( m_surface == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("Shape::_compile '%s' can`t setup surface"
                , this->getName().c_str()
                );

            return false;
        }

        if( m_surface->compile() == false )
        {
            LOGGER_ERROR( m_serviceProvider )("Shape::_compile '%s' can`t compile surface '%s'"
                , this->getName().c_str()
                , m_surface->getName().c_str()
                );

            return false;
        }

        this->invalidateVerticesLocal();
        this->invalidateVerticesColor();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_release()
    {
        Node::_release();

        m_surface->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_update( float _current, float _timing )
    {
        if( m_surface->update( _current, _timing ) == true )
        {
            this->invalidateVerticesLocal();
            this->invalidateVerticesColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesLocal()
    {
        m_invalidateVerticesLocal = true;

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesWM()
    {
        m_invalidateVerticesWM = true;

		this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesColor()
    {
        m_invalidateVerticesColor = true;                
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateColor()
    {
        Node::_invalidateColor();

        this->invalidateVerticesColor();
    }
}