#	include "Factorable.h"
#	include "Factory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factorable::Factorable()
		: m_factory(0)
        , m_destroy(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable::~Factorable()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Factorable::setFactory( Factory * _factory )
	{
		m_factory = _factory;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factorable::destroy()
	{
        m_destroy = true;

		if( this->_destroy() == false )
		{
            return;
        }

        m_factory->destroyObject( this );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Factorable::_destroy()
	{
		return true;
	}
}
