#	include "CodecEngine.h"

#	include "Interface/CodecInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#   include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CodecService, Menge::CodecEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::CodecEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::~CodecEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _factory )
	{
		m_mapDecoderSystem.insert( std::make_pair(_type, _factory) );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterDecoder( const ConstString& _type )
	{
		m_mapDecoderSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _factory )
	{
		m_mapEncoderSystem.insert( std::make_pair( _type, _factory) );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterEncoder( const ConstString& _type )
	{
		m_mapEncoderSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterfacePtr CodecEngine::createDecoder( const ConstString& _type )
	{
		TMapDecoderSystem::iterator it_found = m_mapDecoderSystem.find( _type );

		if( it_found == m_mapDecoderSystem.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("CodecEngine::createDecoder not found codec '%s'"
                , _type.c_str()
                );

			return nullptr;
		}

		const DecoderFactoryInterfacePtr & decoderFactory = it_found->second;

		DecoderInterfacePtr decoder = decoderFactory->createDecoder();

		if( decoder->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("CodecEngine::createDecoder invalid initialize codec %s"
				, _type.c_str()
				);

			return nullptr;
		}

		return decoder;
	}
    //////////////////////////////////////////////////////////////////////////
    EncoderInterfacePtr CodecEngine::createEncoder( const ConstString& _type )
    {
		TMapEncoderSystem::iterator it_found = m_mapEncoderSystem.find( _type );

		if( it_found == m_mapEncoderSystem.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("CodecEngine::createEncoder not found codec %s"
				, _type.c_str()
				);

			return nullptr;
		}

		const EncoderFactoryInterfacePtr & encoderFactory = it_found->second;

		EncoderInterfacePtr encoder = encoderFactory->createEncoder();

        return encoder;
    }
	//////////////////////////////////////////////////////////////////////////
	bool CodecEngine::registerCodecExt( const String & _ext, const ConstString & _codecType )
	{
		TMapCodecTypes::iterator it_found = m_codecTypes.find( _ext );

		if( it_found != m_codecTypes.end() )
		{
			const ConstString & alredyCodec = it_found->second;

			LOGGER_ERROR( m_serviceProvider )("CodecEngine::registerCodecExt '%s' '%s' alredy registry in '%s'"
				, _ext.c_str()
				, _codecType.c_str()
				, alredyCodec.c_str()
				);

			return false;
		}
		
		m_codecTypes.insert( std::make_pair(_ext, _codecType) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & CodecEngine::findCodecType( const FilePath & _path ) const
	{
		const char * str_path = _path.c_str();

		const char * dot_find = strrchr( str_path, '.' );

		if( dot_find == nullptr )
		{
			return ConstString::none();
		}

		String ext( dot_find + 1 );

		TMapCodecTypes::const_iterator it_found = m_codecTypes.find( ext );

		if( it_found == m_codecTypes.end() )
		{
			return ConstString::none();
		}

		const ConstString & codecType = it_found->second;

		return codecType;
	}
}
