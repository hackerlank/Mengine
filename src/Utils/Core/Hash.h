#	pragma once

#	include <stdint.h>
#	include <stddef.h>

namespace Menge
{
	typedef int64_t HashType;

	namespace Helper
	{
		HashType makeHash( const void * _data, size_t _len );
	}
}