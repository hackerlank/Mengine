#	include "VideoCodecPlugin.h"

//#   include "VideoDecoderGVF.h"
//#   include "VideoDecoderVPX.h"
#	include "VideoDecoderTheora.h"

#   include "Codec/DecoderFactory.h"

#   include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeVideoCodec, Menge::VideoCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecPlugin::_initialize()
	{	        
		//m_decoders.push_back( new DecoderFactory<VideoDecoderGVF>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "gvfVideo")) );
        //m_decoders.push_back( new DecoderFactory<VideoDecoderVPX>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "vpxVideo")) );
		m_decoders.push_back( new DecoderFactory<VideoDecoderTheora>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ogvVideo")) );
		m_decoders.push_back( new DecoderFactory<VideoDecoderTheora>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ogvaVideo")) );

		CODEC_SERVICE( m_serviceProvider )
			->registerCodecExt( "ogv", STRINGIZE_STRING_LOCAL( m_serviceProvider, "ogvVideo" ) );

		CODEC_SERVICE( m_serviceProvider )
			->registerCodecExt( "ogva", STRINGIZE_STRING_LOCAL( m_serviceProvider, "ogvaVideo" ) );
				
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{			
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)
				->registerDecoder( name, (*it) );
		}

		LOGGER_WARNING( m_serviceProvider )("Video Theora Version: %s"
			, theora_version_string()
			);
        
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::_finalize()
	{
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)
				->unregisterDecoder( name );
		}

		m_decoders.clear();
	}
}