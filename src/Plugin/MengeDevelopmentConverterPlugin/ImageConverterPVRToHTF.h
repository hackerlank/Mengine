#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class ImageConverterPVRToHTF
		: public DevelopmentConverter
	{
	public:
		ImageConverterPVRToHTF();
		~ImageConverterPVRToHTF();

	public:
		bool initialize() override;

    public:
		bool convert() override;
    };
}	// namespace Menge
