#	include "ImageConverterPNGToACF.h"

#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/MemoryInterface.h"

#	include "Logger/Logger.h"

#   include "Core/Magic.h"
#   include "Core/FilePath.h"
#	include "Core/MemoryHelper.h"

#   include "Config/Blobject.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	ImageConverterPNGToACF::ImageConverterPNGToACF()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	ImageConverterPNGToACF::~ImageConverterPNGToACF()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ImageConverterPNGToACF::initialize()
	{
        m_convertExt = ".acf";

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ImageConverterPNGToACF::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();            

        ConstString full_input = Helper::concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = Helper::concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );
		        
        InputStreamInterfacePtr stream_intput = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), full_input, false );

		if( stream_intput == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid open input file"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

		if( decoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid create decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}
		
		if( decoder->prepareData( stream_intput ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid prepare decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		ImageCodecOptions decoder_options;
		decoder_options.channels = 1;
		decoder_options.pitch = dataInfo->width;		

		if( decoder->setOptions( &decoder_options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid optionize decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}	
		
		size_t data_size = dataInfo->getSize();

		MemoryCacheBufferInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, data_size, "ImageConverterPNGToACF_data" );

		if( data_buffer == nullptr )
		{
			return false;
		}

		void * data_memory = data_buffer->getMemory();

		if( decoder->decode( data_memory, data_size ) == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid decode"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

        OutputStreamInterfacePtr stream_output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

		if( stream_output == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid open output %s"
				, m_options.inputFileName.c_str()
				, full_output.c_str()
				);

			return false;
		}

		ImageEncoderInterfacePtr encoder = CODEC_SERVICE(m_serviceProvider)
			->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "acfImage") );

		if( encoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid create encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		if( encoder->initialize( stream_output ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid initialize encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageCodecOptions encoder_options;
		encoder_options.channels = 1;
		encoder_options.pitch = dataInfo->width;		

		if( encoder->setOptions( &encoder_options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid optionize encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageCodecDataInfo htfDataInfo;
		htfDataInfo.width = dataInfo->width;
		htfDataInfo.height = dataInfo->height;
		htfDataInfo.depth = 1;
		htfDataInfo.mipmaps = 1;
		htfDataInfo.channels = 1;
		htfDataInfo.format = PF_UNKNOWN;

		size_t encode_byte = encoder->encode( data_memory, data_size, &htfDataInfo );

		if( encode_byte == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPNGToACF::convert: %s invalid encode"
				, m_options.inputFileName.c_str()
				);

			return false;
		}
				
        return true;
	}
}