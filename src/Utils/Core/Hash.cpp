#	include "Hash.h"

namespace Menge
{
	namespace Helper
	{
		HashType makeHash( const void * _data, size_t _len )
		{
			if( _len == 0 )
			{
				return 0;
			}

			const unsigned char * p = (const unsigned char *)_data;

			HashType x = *p << 7;

			for( size_t i = 0; i != _len; ++i )
			{
				x = (1000003 * x) ^ *p++;
			}

			x ^= (HashType)_len;

			if( x == -1 )
			{
				x = -2;
			}

			return x;
		}
	}
}