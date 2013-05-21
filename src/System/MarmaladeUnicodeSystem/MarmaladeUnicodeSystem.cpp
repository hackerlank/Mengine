#	include "MarmaladeUnicodeSystem.h"

#	include "Logger/Logger.h"

#   include <IwUTF8.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Menge::UnicodeSystemInterface, Menge::MarmaladeUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    const int iwutf8_invalid_cast = -1;
	//////////////////////////////////////////////////////////////////////////
	MarmaladeUnicodeSystem::MarmaladeUnicodeSystem()
		: m_serviceProvider(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeUnicodeSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeUnicodeSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeUnicodeSystem::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
        const ucs2char * ucs_unicode = reinterpret_cast<const ucs2char *>(_unicode);
        int utf8_size = IwWideCharToUTF8( ucs_unicode, _unicodeSize, _utf8, _utf8Capacity );

        if( utf8_size == iwutf8_invalid_cast )
        {
            return false;
        }

        if( _utf8Size != nullptr )
        {
            size_t u_utf8_size = (size_t)utf8_size;
            *_utf8Size = u_utf8_size;
        }
        
        if( _utf8 != nullptr )
        {
            _utf8[utf8_size] = '\0';
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeUnicodeSystem::utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
	{
        ucs2char * uc_unicode = reinterpret_cast<ucs2char *>(_unicode);
        int wc_size = IwUTF8ToWideChar( _utf8, _utf8Size, uc_unicode, _unicodeCapacity);

        if( wc_size == iwutf8_invalid_cast )
        {
            return false;
        }

        if( _sizeUnicode != nullptr )
        {
            size_t u_wc_size = (size_t)wc_size;
            *_sizeUnicode = u_wc_size;
        }

        if( _unicode != nullptr )
        {
            _unicode[wc_size] = L'\0';
        }

		return true;
	}
}