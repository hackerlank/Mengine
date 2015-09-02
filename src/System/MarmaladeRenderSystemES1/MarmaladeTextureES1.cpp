#	include "MarmaladeTextureES1.h"

#	include "MarmaladeRenderErrorES1.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeTextureES1::MarmaladeTextureES1()
		: m_serviceProvider(nullptr)
		, m_uid(0)
		, m_hwMipmaps(0)
		, m_hwWidth(0)
		, m_hwHeight(0)
        , m_hwChannels(0)
		, m_hwPixelFormat(PF_UNKNOWN)
		, m_internalFormat(GL_RGB)
		, m_format(GL_RGB)
		, m_type(GL_UNSIGNED_BYTE)
		, m_wrapS(GL_CLAMP_TO_EDGE)
		, m_wrapT(GL_CLAMP_TO_EDGE)
		, m_minFilter(GL_LINEAR)
		, m_magFilter(GL_LINEAR)
		, m_mode(ERIM_NORMAL)
		, m_lockMemory(nullptr)
		, m_lockBufferId(INVALID_CACHE_BUFFER_ID)
		, m_lockLevel(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeTextureES1::~MarmaladeTextureES1()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeTextureES1::initialize( ServiceProviderInterface * _serviceProvider, GLuint _uid, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {
		m_serviceProvider = _serviceProvider;
		        
		m_uid = _uid;
		m_mode = _mode;
		m_hwMipmaps = _mipmaps;
        m_hwWidth = _width;
        m_hwHeight = _height;
        m_hwChannels = _channels;
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;
    }
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode MarmaladeTextureES1::getMode() const
	{
		return m_mode;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeTextureES1::getHWWidth() const 
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeTextureES1::getHWHeight() const 
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeTextureES1::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeTextureES1::getHWDepth() const
	{
		return 1;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat MarmaladeTextureES1::getHWPixelFormat() const 
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    void * MarmaladeTextureES1::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

		void * memory;
		CacheBufferID lockBufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( size, &memory, "MarmaladeTexture::lock" );

		if( lockBufferId == INVALID_CACHE_BUFFER_ID )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::lock invalid cache memory %d (l %d w %d h %d c %d f %d)"
				, size
				, _level
				, miplevel_width
				, miplevel_height
				, m_hwChannels
				, m_hwPixelFormat
				);

			return nullptr;
		}

		m_lockBufferId = lockBufferId;
		m_lockMemory = memory;
		
		*_pitch = size / miplevel_height;
		
		m_lockLevel = _level;

		return m_lockMemory;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTextureES1::unlock( uint32_t _level )
	{		
		GLCALL( m_serviceProvider, glBindTexture, ( GL_TEXTURE_2D, m_uid ) );

		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		GLuint textureMemorySize = Helper::getTextureMemorySize(miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat);

		LOGGER_INFO(m_serviceProvider)("MarmaladeTexture::unlock l %d w %d d %d"
			, _level
			, miplevel_width
			, miplevel_height
			, textureMemorySize
			);

		switch( m_internalFormat )
		{
		case GL_ETC1_RGB8_OES:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			{				
				IF_GLCALL( m_serviceProvider, glCompressedTexImage2D, ( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, textureMemorySize, m_lockMemory ) )
				{
					LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::unlock glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
						, _level
						, miplevel_width
						, miplevel_height
						, m_internalFormat
						, m_hwPixelFormat
						, textureMemorySize
						);
				}
			}break;
		default:
			{
				IF_GLCALL( m_serviceProvider, glTexImage2D, ( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, m_format, m_type, m_lockMemory ) )
				{
					LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::unlock glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
						, _level
						, miplevel_width
						, miplevel_height
						, m_internalFormat
						, m_format
						, m_type
						, m_hwPixelFormat
						);
				}
			}break;
		}

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( m_lockBufferId );

		m_lockLevel = 0;
		m_lockBufferId = INVALID_CACHE_BUFFER_ID;
		m_lockMemory = nullptr;		
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTextureES1::_destroy()
	{
		GLCALL( m_serviceProvider, glDeleteTextures, ( 1, &m_uid ) );
	}
	//////////////////////////////////////////////////////////////////////////
	GLuint MarmaladeTextureES1::getUId() const
	{
		return m_uid;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTextureES1::getMinFilter() const
	{
		return m_minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTextureES1::setMinFilter( GLenum _minFilter )
	{
		m_minFilter = _minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTextureES1::getMagFilter() const
	{
		return m_magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTextureES1::setMagFilter( GLenum _magFilter )
	{
		m_magFilter = _magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTextureES1::getWrapS() const
	{
		return m_wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTextureES1::setWrapS( GLenum _wrapS )
	{
		m_wrapS = _wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTextureES1::getWrapT() const
	{
		return m_wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTextureES1::setWrapT( GLenum _wrapT )
	{
		m_wrapT = _wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge