#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class VideoConverterFFMPEGToWEBM
		: public DevelopmentConverter
	{
	public:
		VideoConverterFFMPEGToWEBM();
		~VideoConverterFFMPEGToWEBM();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
    protected:
		bool convert_( const FilePath & _input, const FilePath & _output );
    };
}	// namespace Menge
