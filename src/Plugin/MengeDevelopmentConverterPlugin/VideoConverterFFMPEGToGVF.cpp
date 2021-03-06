#	include "VideoConverterFFMPEGToGVF.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#	include "Logger/Logger.h"

#	include "Core/OutputStreamWriter.h"
#	include "Core/PixelFormat.h"

#   include "WindowsLayer/WindowsIncluder.h"

#   include "lz4.h"
#   include "lz4hc.h"

#	include "gvf_encoder.h"

#	include "stdex/ini.h"

namespace Menge
{
	static const uint32_t DDS_MAGIC = 0x20534444;

	static const uint32_t DDSD_CAPS = 0x00000001;
	static const uint32_t DDSD_HEIGHT = 0x00000002;
	static const uint32_t DDSD_WIDTH = 0x00000004;
	static const uint32_t DDSD_PITCH = 0x00000008;
	static const uint32_t DDSD_PIXELFORMAT = 0x00001000;
	static const uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
	static const uint32_t DDSD_LINEARSIZE = 0x00080000;
	static const uint32_t DDSD_DEPTH = 0x00800000;

	//
	// DDPIXELFORMAT flags
	//
	static const uint32_t DDPF_ALPHAPIXELS = 0x00000001l;
	static const uint32_t DDPF_FOURCC = 0x00000004l;        // Compressed formats
	static const uint32_t DDPF_RGB = 0x00000040l;        // Uncompressed formats
	static const uint32_t DDPF_ALPHA = 0x00000002l;
	static const uint32_t DDPF_COMPRESSED = 0x00000080l;
	static const uint32_t DDPF_LUMINANCE = 0x00020000l;
	static const uint32_t DDPF_BUMPLUMINANCE = 0x00040000l;        // L,U,V
	static const uint32_t DDPF_BUMPDUDV = 0x00080000l;        // U,V

#pragma pack( push, 1 )
	struct DDS_PIXELFORMAT {
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwABitMask;
	};

	typedef struct {
		uint32_t           dwSize;
		uint32_t           dwFlags;
		uint32_t           dwHeight;
		uint32_t           dwWidth;
		uint32_t           dwPitchOrLinearSize;
		uint32_t           dwDepth;
		uint32_t           dwMipMapCount;
		uint32_t           dwReserved1[11];
		DDS_PIXELFORMAT	 ddspf;
		uint32_t           dwSurfaceFlags;
		uint32_t           dwCubemapFlags;
		uint32_t           dwReserved2[3];
	} DDS_HEADER;
#pragma pack(pop)

	//////////////////////////////////////////////////////////////////////////
	static int s_gvf_write( void * _user, const void * _buffer, uint32_t _size )
	{
		OutputStreamWriter * stream = (OutputStreamWriter *)_user;

		if( stream->write( _buffer, _size ) == false )
		{
			return GVF_WRITE_FAILED;
		}

		return GVF_WRITE_SUCCESSFUL;		
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	VideoConverterFFMPEGToGVF::VideoConverterFFMPEGToGVF()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	VideoConverterFFMPEGToGVF::~VideoConverterFFMPEGToGVF()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToGVF::initialize()
	{
        m_convertExt = ".gvf";

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToGVF::removeTempDir_( const WString & _path )
	{
		WString buffer = L"rmdir /S /Q \"%TEMP%/" + _path + L"\"";

		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->cmd( buffer ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::removeTempDir_: invalid"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToGVF::createTempDir_( const WString & _path )
	{
		WString buffer = L"mkdir \"%TEMP%/" + _path + L"\"";

		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->cmd( buffer ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::createTempDir_: invalid"
				);

			return false;
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToGVF::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        WString unicode_input;
        if( Helper::utf8ToUnicode( m_serviceProvider, full_input, unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;        
		}

		WString unicode_output;
		if( Helper::utf8ToUnicode( m_serviceProvider, full_output, unicode_output ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid convert input utf8 to unicode %s"
				, full_input.c_str()
				);

			return false;        
		}

		WCHAR TempPath[MAX_PATH];
		GetTempPathW(MAX_PATH, TempPath);

		//remove temp dir
		if( this->removeTempDir_( L"gvf_pngs" ) == false )
		{
			return false;
		}

		if( this->removeTempDir_( L"gvf_pngs_rgb" ) == false )
		{
			return false;
		}

		if( this->removeTempDir_( L"gvf_pngs_alpha" ) == false )
		{
			return false;
		}

		//create temp dir
		if( this->createTempDir_( L"gvf_pngs" ) == false )
		{
			return false;
		}

		if( this->createTempDir_( L"gvf_pngs_rgb" ) == false )
		{
			return false;
		}

		if( this->createTempDir_( L"gvf_pngs_alpha" ) == false )
		{
			return false;
		}

		LOGGER_WARNING(m_serviceProvider)("gvf try get fps");

		//get fps 
		{
			WString buffer = L"ffprobe -v quiet -print_format ini -show_streams -i \"" + unicode_input + L"\" > \"%TEMP%/gvf_pngs/info.ini\"";
			//WString buffer = L"ffmpeg -loglevel error -i \"" + unicode_input + L"\" -r 24 -f image2 \"%TEMP%/gvf_pngs/frame_%3d.png\"";
			
			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}

		uint32_t fps = 0;
		uint32_t alpha = 0;

		{
			WCHAR INI_Path[MAX_PATH];
			swprintf_s( INI_Path, MAX_PATH, L"%sgvf_pngs\\info.ini", TempPath );

			String utf8_INI_Path;
			Helper::unicodeToUtf8( m_serviceProvider, INI_Path, utf8_INI_Path );

			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_INI_Path), false );
			
			size_t size = stream->size();

			char * buffer = new char[size];
			stream->read( buffer, size );
			stream = nullptr;

			stdex::ini ini;

			if( ini.load( buffer ) == false )
			{
				return false;
			}

			const char * r_frame_rate = ini.getSettingValue( "streams.stream.0", "r_frame_rate" );

			uint32_t frame_count;

			if( sscanf( r_frame_rate, "%d/%d"
				, &fps
				, &frame_count ) != 2 )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: frame rate %s"
					, r_frame_rate
					);
				
				return false;
			}

			fps /= frame_count;

			const char * codec_name = ini.getSettingValue( "streams.stream.0", "codec_name" );

			if( strcmp( codec_name, "vp6a" ) == 0 )
			{
				alpha = 1;
			}
		}

		LOGGER_WARNING(m_serviceProvider)("fps %d"
			, fps
			);		        

		LOGGER_WARNING(m_serviceProvider)("gvf create pngs");

		//create pngs
		if( alpha == 1 )
		{
			WString ws_fps;
			Utils::unsignedToWString( fps, ws_fps );
			WString buffer = L"ffmpeg -loglevel error -i \"" + unicode_input + L"\" -r " + ws_fps + L" -f image2 \"%TEMP%/gvf_pngs/frame_%3d.png\"";

			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}
		else
		{
			WString ws_fps;
			Utils::unsignedToWString( fps, ws_fps );
			WString buffer = L"ffmpeg -loglevel error -i \"" + unicode_input + L"\" -r " + ws_fps + L" -pix_fmt rgb24 -f image2 \"%TEMP%/gvf_pngs_rgb/frame_%3d.png\"";

			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}

		uint32_t pngs_index = 1;
						
		{
			while( true, true )
			{			
				WCHAR PNG_Path[MAX_PATH];
				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.png", TempPath, pngs_index );

				DWORD dwAttrib = GetFileAttributesW( PNG_Path );

				if( dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
				{
					break;
				}

				++pngs_index;
			}
		}

		uint32_t frame_count = pngs_index - 1;

		LOGGER_WARNING(m_serviceProvider)("gvf frames %d"
			, frame_count
			);

		uint32_t width = 0;
		uint32_t height = 0;
		
		if( alpha == 1 )
		{
			LOGGER_WARNING(m_serviceProvider)("gvf split rgb and alpha");

			for( uint32_t index = 0; index != frame_count; ++index )
			{
				WCHAR PNG_Path[MAX_PATH];
				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.png", TempPath, index + 1 );

				String utf8_PNG_Path;
				Helper::unicodeToUtf8( m_serviceProvider, PNG_Path, utf8_PNG_Path );

				InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
					->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_PNG_Path), false );

				if( stream == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image stream for file '%s' was not found"
						, m_options.inputFileName.c_str() 
						);

					return false;
				}

				ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
					->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

				if( imageDecoder == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image decoder for file '%s' was not found"
						, m_options.inputFileName.c_str() 
						);

					return false;
				}

				if( imageDecoder->prepareData( stream ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image initialize for file '%s' was not found"
						, m_options.inputFileName.c_str() 
						);

					return false;
				}

				const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

				width = dataInfo->width;
				height = dataInfo->height;

				size_t bufferSizeRGBA = width * height * dataInfo->channels;
				unsigned char * bufferMemoryRGBA = new unsigned char[bufferSizeRGBA];

				ImageCodecOptions options;

				options.channels = dataInfo->channels;
				options.pitch = dataInfo->width * dataInfo->channels;			 

				if( imageDecoder->decode( bufferMemoryRGBA, bufferSizeRGBA ) == 0 )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image '%s' invalid decode"
						, m_options.inputFileName.c_str() 
						);

					return false;
				}

				{
					WCHAR PNG_RGB_Path[MAX_PATH];
					swprintf_s( PNG_RGB_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frame_%03d.png", TempPath, index + 1 );

					String utf8_PNG_RGB_Path;
					Helper::unicodeToUtf8( m_serviceProvider, PNG_RGB_Path, utf8_PNG_RGB_Path );

					OutputStreamInterfacePtr streamRGB = FILE_SERVICE(m_serviceProvider)
						->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_PNG_RGB_Path) );

					if( streamRGB == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image stream for file '%s' was not found"
							, utf8_PNG_RGB_Path.c_str() 
							);

						return false;
					}

					ImageEncoderInterfacePtr imageEncoderRGB = CODEC_SERVICE(m_serviceProvider)
						->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

					if( imageEncoderRGB == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image encoder for file '%s' was not found"
							, utf8_PNG_RGB_Path.c_str() 
							);

						return false;
					}

					if( imageEncoderRGB->initialize( streamRGB ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: can't initialize encoder for filename '%s'"
							, utf8_PNG_RGB_Path.c_str() 
							);

						return false;
					}

					ImageCodecDataInfo dataInfoRGB;
					//dataInfo.format = _image->getHWPixelFormat();
					dataInfoRGB.width = width;
					dataInfoRGB.height = height;		
					dataInfoRGB.channels = 3;
					dataInfoRGB.depth = 1;				
					dataInfoRGB.mipmaps = 1;

					ImageCodecOptions options;		

					options.channels = 3;
					options.pitch = dataInfoRGB.width * 3;

					imageEncoderRGB->setOptions( &options );

					size_t bufferSizeRGB = dataInfoRGB.width * dataInfoRGB.height * dataInfoRGB.channels;
					unsigned char * bufferMemoryRGB = new unsigned char[bufferSizeRGB];

					for( uint32_t j = 0; j != height; ++j )
					{
						for( uint32_t i = 0; i != width; ++i )
						{
							unsigned char * pitchMemoryRGBA = bufferMemoryRGBA + (j * width + i) * 4;
							unsigned char * pitchMemoryRGB = bufferMemoryRGB + (j * width + i) * 3;

							pitchMemoryRGB[0] = pitchMemoryRGBA[0];
							pitchMemoryRGB[1] = pitchMemoryRGBA[1];
							pitchMemoryRGB[2] = pitchMemoryRGBA[2];
						}
					}

					if( imageEncoderRGB->encode( bufferMemoryRGB, &dataInfoRGB ) == 0 )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: can't encode '%s'"
							, utf8_PNG_RGB_Path.c_str() 
							);

						return false;
					}

					delete bufferMemoryRGB;
				}

				{
					WCHAR PNG_Alpha_Path[MAX_PATH];
					swprintf_s( PNG_Alpha_Path, MAX_PATH, L"%sgvf_pngs_alpha\\frame_%03d.png", TempPath, index + 1 );

					String utf8_PNG_Alpha_Path;
					Helper::unicodeToUtf8( m_serviceProvider, PNG_Alpha_Path, utf8_PNG_Alpha_Path );

					OutputStreamInterfacePtr streamAlpha = FILE_SERVICE(m_serviceProvider)
						->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_PNG_Alpha_Path) );

					if( streamAlpha == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image stream for file '%s' was not found"
							, utf8_PNG_Alpha_Path.c_str() 
							);

						return false;
					}

					ImageEncoderInterfacePtr imageEncoderAlpha = CODEC_SERVICE(m_serviceProvider)
						->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

					if( imageEncoderAlpha == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image encoder for file '%s' was not found"
							, utf8_PNG_Alpha_Path.c_str() 
							);

						return false;
					}

					if( imageEncoderAlpha->initialize( streamAlpha ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: can't initialize encoder for filename '%s'"
							, utf8_PNG_Alpha_Path.c_str() 
							);

						return false;
					}

					ImageCodecDataInfo dataInfoAlpha;
					//dataInfo.format = _image->getHWPixelFormat();
					dataInfoAlpha.width = width;
					dataInfoAlpha.height = height;		
					dataInfoAlpha.channels = 1;
					dataInfoAlpha.depth = 1;				
					dataInfoAlpha.mipmaps = 1;

					ImageCodecOptions options;		

					options.channels = 1;
					options.pitch = dataInfoAlpha.width;

					imageEncoderAlpha->setOptions( &options );

					size_t bufferSizeAlpha = dataInfoAlpha.width * dataInfoAlpha.height * dataInfoAlpha.channels;
					unsigned char * bufferMemoryAlpha = new unsigned char[bufferSizeAlpha];

					for( uint32_t j = 0; j != height; ++j )
					{
						for( uint32_t i = 0; i != width; ++i )
						{
							unsigned char * pitchMemoryRGBA = bufferMemoryRGBA + (j * width + i) * 4;
							unsigned char * pitchMemoryAlpha = bufferMemoryAlpha + (j * width + i) * 1;

							pitchMemoryAlpha[0] = pitchMemoryRGBA[0];
							pitchMemoryAlpha[1] = pitchMemoryRGBA[1];
							pitchMemoryAlpha[2] = pitchMemoryRGBA[2];
						}
					}

					if( imageEncoderAlpha->encode( bufferMemoryAlpha, &dataInfoAlpha ) == 0 )
					{
						LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: can't encode '%s'"
							, utf8_PNG_Alpha_Path.c_str() 
							);

						return false;
					}

					delete bufferMemoryAlpha;
				}
			}
		}

		uint32_t width_pow2 = 0;
		uint32_t height_pow2 = 0;

		LOGGER_WARNING(m_serviceProvider)("gvf create htf");

		if( m_options.params.find("-dxt1") != String::npos )
		{			
			const uint32_t process_count = 4;

			OutputStreamInterfacePtr pngs_stream[process_count];

			//create dds
			for( uint32_t process = 0; process != process_count; ++process )
			{
				WCHAR PNGS_Path[MAX_PATH];
				swprintf_s( PNGS_Path, MAX_PATH, L"%sgvf_pngs\\frames_%d.txt", TempPath, process );

				String utf8_pngs_path;
				Helper::unicodeToUtf8(m_serviceProvider, PNGS_Path, utf8_pngs_path);

				pngs_stream[process] = FILE_SERVICE(m_serviceProvider)
					->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_pngs_path) );
			}

			uint32_t process_index = 0;
			for( uint32_t index = 1; index != pngs_index; ++index )
			{	
				WCHAR PNG_Path[MAX_PATH];

				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frame_%03d.png\n", TempPath, index );

				String utf8_png_path;
				Helper::unicodeToUtf8(m_serviceProvider, PNG_Path, utf8_png_path);				

				uint32_t process_current = (process_index++) % process_count;

				pngs_stream[process_current]->write( utf8_png_path.c_str(), utf8_png_path.size() );
			}

			for( uint32_t process = 0; process != process_count; ++process )
			{
				pngs_stream[process]->flush();
				pngs_stream[process] = nullptr;
			}

			WString buffer = L" -quiet -mipMode None ";

			buffer += L" -fileformat dds ";
			buffer += L" -DXT1 ";
			buffer += L" -dxtQuality superfast ";			
			buffer += L" -rescalemode hi ";
			buffer += L" -outsamedir ";
			buffer += L" -quality 50 ";

			PROCESS_INFORMATION lpProcessInformation[process_count];

			for( uint32_t process = 0; process != process_count; ++process )
			{
				WString process_buffer = buffer;

				WCHAR PNGS_Path[MAX_PATH];
				swprintf_s( PNGS_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frames_%d.txt", TempPath, process );

				process_buffer += L" -file @";
				process_buffer += PNGS_Path;

				WCHAR lpCommandLine[MAX_PATH];
				swprintf_s( lpCommandLine, MAX_PATH, L"%s", process_buffer.c_str() );

				WCHAR curDir[MAX_PATH];
				GetCurrentDirectoryW( MAX_PATH, curDir );

				WCHAR lpApplicationName[MAX_PATH];
				swprintf_s( lpApplicationName, L"%s\\tools\\crunch\\crunch_x64.exe", curDir );

				STARTUPINFOW lpStartupInfo;
				ZeroMemory( &lpStartupInfo, sizeof(lpStartupInfo) );
				lpStartupInfo.cb = sizeof(lpStartupInfo);

				ZeroMemory( &lpProcessInformation[process], sizeof(lpProcessInformation[process]) );

				BOOL successful = CreateProcessW( lpApplicationName
					, lpCommandLine
					, NULL
					, NULL
					, FALSE
					, BELOW_NORMAL_PRIORITY_CLASS
					, NULL
					, NULL
					, &lpStartupInfo
					, &lpProcessInformation[process] );

				if( successful == FALSE )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid run process %ls %ls"
						, lpApplicationName
						, lpCommandLine
						);

					return false;
				}

				CloseHandle( lpProcessInformation[process].hThread );
			}

			for( uint32_t process = 0; process != process_count; ++process )
			{
				WaitForSingleObject( lpProcessInformation[process].hProcess, INFINITE );

				DWORD exit_code;//��� ���������� ��������
				GetExitCodeProcess( lpProcessInformation[process].hProcess, &exit_code );

				CloseHandle( lpProcessInformation[process].hProcess );
			}

			{
				WCHAR PNG_Path[MAX_PATH];
				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frame_%03d.dds", TempPath, 1 );

				String utf8_PNG_Path;
				Helper::unicodeToUtf8( m_serviceProvider, PNG_Path, utf8_PNG_Path );

				InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
					->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_PNG_Path), false );

				ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
					->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "crnImage") );

				if( imageDecoder == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image decoder for file '%s' was not found"
						, utf8_PNG_Path.c_str() 
						);

					return false;
				}

				if( imageDecoder->prepareData( stream ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image initialize for file '%s' was not found"
						, utf8_PNG_Path.c_str() 
						);

					return false;
				}

				const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

				width_pow2 = dataInfo->width;
				height_pow2 = dataInfo->height;
			}
		}
		else if( m_options.params.find("-etc1") != String::npos )
		{			
			for( uint32_t index = 1; index != pngs_index; ++index )
			{	
				WCHAR PNG_Path[MAX_PATH];
				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frame_%03d.png\n", TempPath, index );

				WCHAR PVR_Path[MAX_PATH];
				swprintf_s( PVR_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frame_%03d.pvr\n", TempPath, index );

				/*				String utf8_png_path;
				Helper::unicodeToUtf8(m_serviceProvider, PNG_Path, utf8_png_path);	*/			
				
				WString buffer;
				buffer += L" -i " + WString(PNG_Path) + L" ";
				buffer += L" -o " + WString(PVR_Path) + L" ";
				buffer += L" -f ETC1 ";
				buffer += L" -q pvrtcfast ";			
				buffer += L" -dither ";
				buffer += L" -square + ";
				buffer += L" -pot + ";
			
				WCHAR lpCommandLine[32768];
				swprintf_s( lpCommandLine, MAX_PATH, L"%s", buffer.c_str() );

				WCHAR curDir[MAX_PATH];
				GetCurrentDirectoryW( MAX_PATH, curDir );

				WCHAR lpApplicationName[MAX_PATH];
				swprintf_s( lpApplicationName, L"%s\\tools\\pvrtool\\PVRTexTool.exe", curDir );

				STARTUPINFOW lpStartupInfo;
				ZeroMemory( &lpStartupInfo, sizeof(lpStartupInfo) );
				lpStartupInfo.cb = sizeof(lpStartupInfo);

				PROCESS_INFORMATION lpProcessInformation;
				ZeroMemory( &lpProcessInformation, sizeof(lpProcessInformation) );

				BOOL successful = CreateProcessW( lpApplicationName
					, lpCommandLine
					, NULL
					, NULL
					, FALSE
					, BELOW_NORMAL_PRIORITY_CLASS
					, NULL
					, NULL
					, &lpStartupInfo
					, &lpProcessInformation );

				if( successful == FALSE )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid run process %ls %ls"
						, lpApplicationName
						, lpCommandLine
						);

					return false;
				}

				CloseHandle( lpProcessInformation.hThread );
				WaitForSingleObject( lpProcessInformation.hProcess, INFINITE );

				DWORD exit_code;//��� ���������� ��������
				GetExitCodeProcess( lpProcessInformation.hProcess, &exit_code );

				CloseHandle( lpProcessInformation.hProcess );
			}

			{
				WCHAR PVR_Path[MAX_PATH];
				swprintf_s( PVR_Path, MAX_PATH, L"%sgvf_pngs_rgb\\frame_%03d.pvr", TempPath, 1 );

				String utf8_PVR_Path;
				Helper::unicodeToUtf8( m_serviceProvider, PVR_Path, utf8_PVR_Path );

				InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
					->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_PVR_Path), false );

				ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
					->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "pvrImage") );

				if( imageDecoder == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image decoder for file '%s' was not found"
						, utf8_PVR_Path.c_str() 
						);

					return false;
				}

				if( imageDecoder->prepareData( stream ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image initialize for file '%s' was not found"
						, utf8_PVR_Path.c_str() 
						);

					return false;
				}

				const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

				width_pow2 = dataInfo->width;
				height_pow2 = dataInfo->height;
			}
		}



		{

		}

		LOGGER_WARNING(m_serviceProvider)("gvf save file size %d %d frame %d %d"
			, width
			, height
			, width_pow2
			, height_pow2
			);

		gvf_encoder * gvf;
		gvf_error_t err_code;

		err_code = gvf_encoder_create( &gvf );

		if( err_code != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_create err code %d"
				, m_options.inputFileName.c_str() 
				, err_code
				);

			return false;
		}

		uint16_t gvf_format = alpha == 1 ? GVF_FORMAT_DXT5 : GVF_FORMAT_DXT1;
		uint16_t gvf_compression = GVF_COMPRESSION_LZ4;

		err_code = gvf_encoder_header( gvf, gvf_format, gvf_compression, width, height, width_pow2, height_pow2, fps, frame_count );

		if( err_code != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_header err code %d"
				, m_options.inputFileName.c_str() 
				, err_code
				);

			return false;
		}
				
		LOGGER_WARNING(m_serviceProvider)("gvf save frames");

		{
			for( uint32_t index = 0; index != frame_count; ++index )
			{
				WCHAR DDS_Path[MAX_PATH];
				swprintf_s( DDS_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.crn", TempPath, index + 1 );

				String utf8_DDS_Path;
				Helper::unicodeToUtf8( m_serviceProvider, DDS_Path, utf8_DDS_Path );

				InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
					->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_DDS_Path), false );

				uint32_t size = stream->size();

				unsigned char * buffer = new unsigned char [size];
				stream->read( buffer, size );
				stream = nullptr;

				err_code = gvf_encoder_frame( gvf, index, buffer, size );

				if( err_code != GVF_ERROR_SUCCESSFUL )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_frame %d err code %d"
						, m_options.inputFileName.c_str() 
						, index
						, err_code
						);

					return false;
				}

				delete [] buffer;
			}
		}

		OutputStreamInterfacePtr gvf_stream = FILE_SERVICE(m_serviceProvider)
			->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, full_output) );

		OutputStreamWriter gvf_wr(gvf_stream);
		
		err_code = gvf_encoder_flush( gvf, &gvf_wr, &s_gvf_write );

		if( err_code != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_flush err code %d"
				, m_options.inputFileName.c_str() 
				, err_code
				);

			return false;
		}

		gvf_encoder_destroy( gvf );

		LOGGER_WARNING(m_serviceProvider)("gvf finish");
		
		//remove temp dir
		if( this->removeTempDir_( L"gvf_pngs" ) == false )
		{
			return false;
		}

		if( this->removeTempDir_( L"gvf_pngs_rgb" ) == false )
		{
			return false;
		}

		if( this->removeTempDir_( L"gvf_pngs_alpha" ) == false )
		{
			return false;
		}

        return true;
	}
}