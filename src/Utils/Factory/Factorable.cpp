#	include "Factorable.h"
#	include "Factory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factorable::Factorable()
		: m_factory(0)
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
		this->_destroy();

		if( m_factory )
		{
			m_factory->destroyObject( this );
		}
		else
		{
			delete this;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Factorable::_destroy()
	{
		//Empty
	}
}