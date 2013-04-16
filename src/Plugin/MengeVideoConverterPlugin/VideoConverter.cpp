#	include "VideoConverter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoConverter::VideoConverter( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	VideoConverter::~VideoConverter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoConverter::setOptions( ConverterOptions * _options )
	{
		m_options = *(_options);
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoConverter::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
}