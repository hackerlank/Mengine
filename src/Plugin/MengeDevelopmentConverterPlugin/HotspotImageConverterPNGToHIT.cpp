#	include "HotspotImageConverterPNGToHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/PickCodecInterface.h"

#	include "Core/MemoryHelper.h"

#	include "Logger/Logger.h"

#   include <math.h>

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	HotspotImageConverterPNGToHIT::HotspotImageConverterPNGToHIT()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	HotspotImageConverterPNGToHIT::~HotspotImageConverterPNGToHIT()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPNGToHIT::initialize()
	{
        m_convertExt = ".hit";

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPNGToHIT::validateVersion( const InputStreamInterfacePtr & _stream ) const
	{
		PickDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<PickDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "hitPick") );

		if( decoder == nullptr )
		{
			return false;
		}

		if( decoder->prepareData( _stream ) == false )
		{
			return false;
		}

		const PickCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		uint32_t bufferSize = (uint32_t)dataInfo->mipmapsize;

		MemoryCacheBufferInterfacePtr memory = Helper::createMemoryCacheBuffer( m_serviceProvider, bufferSize, "HotspotImageConverterPNGToHIT::validateVersion" );

		if( memory == nullptr )
		{
			return false;
		}

		uint8_t * buffer = memory->getMemory();

		size_t decode_mipmapsize = decoder->decode( buffer, bufferSize );
				
		if( decode_mipmapsize != bufferSize )
		{
			return false;                
		}
		
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPNGToHIT::convert()
	{
        InputStreamInterfacePtr input_stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( m_options.pakName, m_options.inputFileName, false );

        if( input_stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_: Image file '%s' was not found"
                , m_options.inputFileName.c_str() 
                );

            return false;
        }

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_: Image decoder for file '%s' was not found"
                , m_options.inputFileName.c_str() 
                );

            return nullptr;
        }

		if( imageDecoder->prepareData( input_stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_: Image initialize for file '%s' was not found"
				, m_options.inputFileName.c_str() 
				);

			return nullptr;
		}

        const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

        ImageCodecOptions optionsAlpha;

        optionsAlpha.flags |= DF_READ_ALPHA_ONLY;
        //optionsAlpha.flags |= DF_CUSTOM_PITCH;
        optionsAlpha.pitch = dataInfo->width;
        optionsAlpha.channels = 1;

        imageDecoder->setOptions( &optionsAlpha );

        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;

        uint32_t mimmap_level;
		uint32_t mimmap_size = this->calcMimMapBufferLevelAndSize_( width, height, mimmap_level );

		uint32_t bufferSize = width * height + mimmap_size;
		
		MemoryCacheBufferInterfacePtr memory = Helper::createMemoryCacheBuffer( m_serviceProvider, bufferSize, "HotspotImageConverterPNGToHIT::convert" );

		if( memory == nullptr )
		{
			return false;
		}

		uint8_t * buffer = memory->getMemory();

        if( imageDecoder->decode( buffer, bufferSize ) == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_ Invalid decode %s"
                , m_options.inputFileName.c_str()                
                );
            
            return false;
        }

        input_stream = nullptr;

        this->makeMipMapLevel_( buffer, width, height, mimmap_level );

        OutputStreamInterfacePtr output_stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( m_options.pakName, m_options.outputFileName );

        if( output_stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_: HIT file '%s' not create (open file %s)"
                , m_options.outputFileName.c_str() 
                , m_options.pakName.c_str()
                );

            return nullptr;
        }

        PickEncoderInterfacePtr encoder = CODEC_SERVICE(m_serviceProvider)
            ->createEncoderT<PickEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "hitPick") );
        
        if( encoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_: HIT file '%s' not create (createEncoder hitPick)"
                , m_options.outputFileName.c_str() 
                );
			
            return nullptr;
        }

		if( encoder->initialize( output_stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_: HIT file '%s' not initialize (createEncoder hitPick)"
				, m_options.outputFileName.c_str() 
				);

			return nullptr;
		}

        PickCodecDataInfo di;

        di.width = width;
        di.height = height;
        di.mipmaplevel = mimmap_level;
        di.mipmapsize = bufferSize;

        encoder->encode( buffer, bufferSize, &di );

		encoder->finalize();
		
        return true;
	}
    //////////////////////////////////////////////////////////////////////////
	uint32_t HotspotImageConverterPNGToHIT::calcMimMapBufferLevelAndSize_( uint32_t _width, uint32_t _height, uint32_t & _level )
    {
        uint32_t mipmap_pow_width = (uint32_t)(logf( (float)_width ) / logf( 2.f ));
        uint32_t mipmap_pow_height = (uint32_t)(logf( (float)_height ) / logf( 2.f ));

        uint32_t mipmap_level = (std::min)(mipmap_pow_width, mipmap_pow_height) + 1;

		uint32_t bufferOffset = 0;
        for( uint32_t i = 1; i != mipmap_level; ++i )
        {
			uint32_t offset = (_width >> i) * (_height >> i);
            bufferOffset += offset;
        }

        _level = mipmap_level;

        return bufferOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotspotImageConverterPNGToHIT::makeMipMapLevel_( uint8_t * _buffer, uint32_t _width, uint32_t _height, uint32_t _level )
    {
        uint8_t * mipmap[32];
        
        mipmap[0] = _buffer;

		uint32_t bufferOffset = _width * _height;
        for( uint32_t i = 1; i != _level; ++i )
        {
            mipmap[i] = _buffer + bufferOffset;

			uint32_t offset = (_width >> i) * (_height >> i);
            bufferOffset += offset;
        }

        uint32_t mipmap_width = _width;
        uint32_t mipmap_height = _height;

        for( uint32_t
            it = 1,
            it_end = _level;
        it != it_end;
        ++it )
        {
            uint32_t prev_width = mipmap_width >> (it - 1);
            uint32_t prev_height = mipmap_height >> (it - 1);

            uint32_t cur_width = mipmap_width >> it;
            uint32_t cur_height = mipmap_height >> it;

			uint8_t * prev_buff = mipmap[it - 1];
			uint8_t * cur_buff = mipmap[it];

            for( uint32_t i = 0; i != cur_width; ++i )
            {
                for( uint32_t j = 0; j != cur_height; ++j )
                {
                    uint32_t i0 = (i * 2 + 0) + (j * 2 + 0) * prev_width;
                    uint32_t i1 = (i * 2 + 1) + (j * 2 + 0) * prev_width;
                    uint32_t i2 = (i * 2 + 0) + (j * 2 + 1) * prev_width;
                    uint32_t i3 = (i * 2 + 1) + (j * 2 + 1) * prev_width;

                    uint8_t v0 = prev_buff[i0];
                    uint8_t v1 = prev_buff[i1];
                    uint8_t v2 = prev_buff[i2];
                    uint8_t v3 = prev_buff[i3];

                    uint8_t b0 = (std::max)( v0, v1 );
                    uint8_t b1 = (std::max)( v2, v3 );

                    uint8_t b_max = (std::max)( b0, b1 );

					if( i == (cur_width - 1) && (prev_width % 2) == 1 )
					{
						uint32_t i4 = (i * 2 + 2) + (j * 2 + 0) * prev_width;
						uint32_t i5 = (i * 2 + 2) + (j * 2 + 1) * prev_width;

						uint8_t v4 = prev_buff[i4];
						uint8_t v5 = prev_buff[i5];

						uint8_t b3 = (std::max)( v4, v5 );

						b_max = (std::max)( b_max, b3 );
					}

					if( j == (cur_height - 1) && (prev_height % 2) == 1 )
					{
						uint32_t i4 = (i * 2 + 0) + (j * 2 + 2) * prev_width;
						uint32_t i5 = (i * 2 + 1) + (j * 2 + 2) * prev_width;

						uint8_t v4 = prev_buff[i4];
						uint8_t v5 = prev_buff[i5];

						uint8_t b3 = (std::max)( v4, v5 );

						b_max = (std::max)( b_max, b3 );
					}

					if( i == (cur_width - 1) && (prev_width % 2) == 1 && 
						j == (cur_height - 1) && (prev_height % 2) == 1 )
					{
						uint32_t i4 = (i * 2 + 2) + (j * 2 + 2) * prev_width;

						uint8_t v4 = prev_buff[i4];

						b_max = (std::max)( b_max, v4 );
					}

                    cur_buff[ i + j * cur_width ] = b_max;
                }
            }
        }
    }
}