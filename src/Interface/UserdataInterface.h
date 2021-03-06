#   pragma once

#	include "Interface/ServiceInterface.h"
#   include "Interface/MemoryInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "Core/Magic.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DECLARE_MAGIC_NUMBER( MAGIC_USER_DATA, 'G', 'M', 'D', '1', 1 );
	//////////////////////////////////////////////////////////////////////////
	class UserdataServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "UserdataService" );

	public:
		virtual bool addUserdata( const ConstString & _name, const ConstString & _category, const FilePath & path ) = 0;
		virtual bool removeUserdata( const ConstString & _name ) = 0;
		virtual bool hasUserdata( const ConstString & _name ) const = 0;

	public:
		virtual MemoryCacheBufferInterfacePtr loadUserdata( const ConstString & _name ) const = 0;
		virtual bool writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const = 0;
	};

#   define USERDATA_SERVICE( serviceProvider )\
    ((Menge::UserdataServiceInterface*)SERVICE_GET(serviceProvider, Menge::UserdataServiceInterface))
}