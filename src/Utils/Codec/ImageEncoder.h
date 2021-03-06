#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "Codec/Encoder.h"

namespace Menge
{
	class ImageEncoder
		: public Encoder<ImageEncoderInterface>
	{
	public:
		ImageEncoder();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

	protected:
		ImageCodecOptions m_options;
	};
}
