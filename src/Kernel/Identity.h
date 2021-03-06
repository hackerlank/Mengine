#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class Identity
	{
	public:
		Identity();

	public:
		void setName( const ConstString & _name );
		inline const ConstString & getName() const;

        void setType( const ConstString & _type );
        inline const ConstString & getType() const;
		
	protected:
		ConstString m_name;
		ConstString m_type;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & Identity::getName()const
	{	
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & Identity::getType() const
	{
		return m_type;
	}
}
