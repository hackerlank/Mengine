#	pragma once

#	include "Interface/CodecInterface.h"

#   include "Core/FilePath.h"

namespace Menge
{
	struct XmlCodecDataInfo
		: public CodecDataInfo
	{
	};

	struct XmlCodecOptions
		: public CodecOptions
	{
		ConstString pathProtocol;
		ConstString pathXml;
		ConstString pathBin;
		//int version;
	};

	class XmlDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const XmlCodecDataInfo * getCodecDataInfo() const override = 0;
	};
    
    typedef stdex::intrusive_ptr<XmlDecoderInterface> XmlDecoderInterfacePtr;
}	
