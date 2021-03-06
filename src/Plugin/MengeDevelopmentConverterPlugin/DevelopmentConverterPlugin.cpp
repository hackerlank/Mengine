#	include "DevelopmentConverterPlugin.h"

#	include "Interface/StringizeInterface.h"

#	include "HotspotImageConverterPNGToHIT.h"
#	include "ParticleConverterPTCToPTZ.h"
#	include "ImageConverterPVRToHTF.h"
#	include "ImageConverterDDSToHTF.h"
#	include "ImageConverterPNGToACF.h"
#	include "SoundConverterFFMPEGToOGG.h"
#	include "VideoConverterFFMPEGToWEBM.h"
#	include "VideoConverterFFMPEGToOGV.h"
#	include "VideoConverterFFMPEGToOGVA.h"
#	include "MovieKeyConverterXMLToAEK.h"
#	include "ModelConverterMDLToMDZ.h"

#   include "Logger/Logger.h"

#   include "Codec/ConverterFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevelopmentConverter, Menge::DevelopmentConverterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DevelopmentConverterPlugin::DevelopmentConverterPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DevelopmentConverterPlugin::_initialize()
	{
		m_converters.push_back( new ConverterFactory<HotspotImageConverterPNGToHIT>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "png2hit")) );
		m_converters.push_back( new ConverterFactory<SoundConverterFFMPEGToOGG>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ffmpegToOggSound")) );
		m_converters.push_back( new ConverterFactory<VideoConverterFFMPEGToWEBM>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ffmpegToWebM")) );
		m_converters.push_back( new ConverterFactory<VideoConverterFFMPEGToOGV>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ffmpegToOGV")) );
		m_converters.push_back( new ConverterFactory<VideoConverterFFMPEGToOGVA>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ffmpegToOGVA")) );
		m_converters.push_back( new ConverterFactory<MovieKeyConverterXMLToAEK>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "xmlToAekMovie")) );
		m_converters.push_back( new ConverterFactory<ModelConverterMDLToMDZ>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "mdlToMdzModel")) );
		m_converters.push_back( new ConverterFactory<ParticleConverterPTCToPTZ>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ptc2ptz")) );
		m_converters.push_back( new ConverterFactory<ImageConverterPVRToHTF>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "pvr2htf")) );
		m_converters.push_back( new ConverterFactory<ImageConverterDDSToHTF>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "dds2htf")) );		
		m_converters.push_back( new ConverterFactory<ImageConverterPNGToACF>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "png2acf")) );

		for( TVectorHotspotImageConverters::iterator
			it = m_converters.begin(), 
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
            ConverterFactoryInterface * converter = (*it);

			const ConstString & name = converter->getName();

			CONVERTER_SERVICE(m_serviceProvider)
                ->registerConverter( name, converter );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DevelopmentConverterPlugin::_finalize()
	{
		for( TVectorHotspotImageConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
			ConverterFactoryInterface * converter = (*it);

			const ConstString & name = converter->getName();

			CONVERTER_SERVICE(m_serviceProvider)
				->unregisterConverter( name );
		}

		m_converters.clear();
	}
}