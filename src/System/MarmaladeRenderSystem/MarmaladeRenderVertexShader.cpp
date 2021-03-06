#	include "MarmaladeRenderVertexShader.h"

#	include "MarmaladeRenderError.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderVertexShader::MarmaladeRenderVertexShader()
		: m_serviceProvider( nullptr )
		, m_shaderId( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderVertexShader::~MarmaladeRenderVertexShader()
	{
		if( m_shaderId != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteShader, (m_shaderId) );
			m_shaderId = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderVertexShader::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * MarmaladeRenderVertexShader::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & MarmaladeRenderVertexShader::getName() const
	{
		return m_name;
	}
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderVertexShader::initialize( const ConstString & _name, const void * _source, size_t _size, bool _isCompile )
	{
		m_name = _name;

		GLuint shaderId;		
		GLCALLR( m_serviceProvider, shaderId, glCreateShader, (GL_VERTEX_SHADER) );

		if( shaderId == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeShader::initialize %s invalid create shader"
				, _name.c_str()
				);

			return false;
		}

		if( _isCompile == false )
		{
			const char * str_source = static_cast<const char *>(_source);
			GLint str_size = (GLint)_size;

			GLCALL( m_serviceProvider, glShaderSource, ( shaderId, 1, &str_source, &str_size ) );
			GLCALL( m_serviceProvider, glCompileShader, ( shaderId ) );

			int32 device = s3eDeviceGetInt( S3E_DEVICE_OS );
			int32 deviceFeatureLevel = s3eDeviceGetInt( S3E_DEVICE_DX_FEATURE_LEVEL );

			if( (device != S3E_OS_ID_WS8 && device != S3E_OS_ID_WS81 && device != S3E_OS_ID_WP81) ||
				deviceFeatureLevel < 93 )
			{
				IwCompileShadersPlatformType shadersPlatformType = IwGetCompileShadersPlatformType();
				// This is needed to compile shaders for Windows Store 8/8.1 and Windows Phone 8.1 using the Win32 Marmalade Simulator.
				// For more information look at README.IwGLES2.txt.
				if( shadersPlatformType == IW_CS_OS_ID_WS8 ||
					shadersPlatformType == IW_CS_OS_ID_WS81 ||
					shadersPlatformType == IW_CS_OS_ID_WP81 )
				{
					s3eRegisterShader( str_source, IW_GL_ST_VERTEX, IW_DX_FL_9_3 );
				}
			}
		}
		else
		{
			GLCALL( m_serviceProvider, glShaderBinary, (1, &shaderId, GL_SHADER_BINARY_ANGLE_DX11, _source, _size) );
		}

		GLint status;
		GLCALL( m_serviceProvider, glGetShaderiv, ( shaderId, GL_COMPILE_STATUS, &status ) );

		if( status == GL_FALSE )
		{
			GLchar errorLog[1024];
			GLCALL( m_serviceProvider, glGetShaderInfoLog, ( shaderId, 1023, NULL, errorLog ) );

			LOGGER_ERROR(m_serviceProvider)("MarmaladeShader::initialize compilation shader error '%s'"
				, errorLog
				);			

			return false;
		}

		m_shaderId = shaderId;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderVertexShader::attach( GLuint _program )
	{ 
		GLCALL( m_serviceProvider, glAttachShader, ( _program, m_shaderId ) );
	}
}	// namespace Menge
