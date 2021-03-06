#	ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN

#	ifndef NOMINMAX
#	define NOMINMAX
#	endif

#pragma warning(push, 0) 
#	include <Windows.h>
#   include <WinUser.h>

#   include <shellapi.h>
#   include <shlwapi.h>
#   include <shlobj.h>
#	include <fcntl.h>
#pragma warning(pop) 

#pragma comment ( lib, "Ws2_32.lib" )
#pragma comment ( lib, "Crypt32.lib" )

#   include <iostream>

#	include "Interface/ServiceInterface.h"

#	include "XmlToBinDecoder.h"
#	include "AlphaSpreading.h"
#	include "XmlToAekConverter.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/LoggerInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/MemoryInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ImageCodecInterface.h"
#	include "Interface/LoaderInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#	include "Codec/ConverterFactory.h"

#	include "Image.h"

#   include "Logger\Logger.h"

#	include <pybind/pybind.hpp>
#	include "stdex/sha1.h"

#	include <io.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( MengeWin32FileGroup );
PLUGIN_EXPORT( MengeImageCodec );
PLUGIN_EXPORT( MengeZip );
PLUGIN_EXPORT( MengeLZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( UnicodeService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );

SERVICE_EXTERN( WindowsLayer );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( LoaderService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	ServiceProviderInterface * serviceProvider = NULL;
		
	//////////////////////////////////////////////////////////////////////////
	static void createConsole()
	{
		typedef BOOL (WINAPI *PATTACHCONSOLE)(DWORD);

		PATTACHCONSOLE pAttachConsole = nullptr;

		HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

		if( hKernel32 != nullptr )
		{
			FARPROC proc = GetProcAddress( hKernel32, "AttachConsole" );

			pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(proc);

			if( pAttachConsole == nullptr )
			{
				FreeLibrary( hKernel32 );
				return;
			}
		}


		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		// try to attach to calling console first
		if( pAttachConsole( (DWORD)-1 ) == FALSE )
		{
			// allocate a console for this app
			AllocConsole();

			// set the screen buffer to be big enough to let us scroll text
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
			coninfo.dwSize.Y = 1000;
			SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
		}

		FILE *fp[3];
		HANDLE lStdHandle[3];
		int hConHandle[3];

		// redirect unbuffered STDOUT to the console
		lStdHandle[0] = GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle[0] = _open_osfhandle((intptr_t)lStdHandle[0], _O_TEXT);
		fp[0] = _fdopen( hConHandle[0], "w" );
		*stdout = *fp[0];
		setvbuf( stdout, NULL, _IONBF, 0 );

		// redirect unbuffered STDIN to the console
		lStdHandle[1] = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle[1] = _open_osfhandle((intptr_t)lStdHandle[1], _O_TEXT);
		fp[1] = _fdopen( hConHandle[1], "r" );
		*stdin = *fp[1];
		setvbuf( stdin, NULL, _IONBF, 0 );

		// redirect unbuffered STDERR to the console
		lStdHandle[2] = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle[2] = _open_osfhandle((intptr_t)lStdHandle[2], _O_TEXT);
		fp[2] = _fdopen( hConHandle[2], "w" );
		*stderr = *fp[2];
		setvbuf( stderr, NULL, _IONBF, 0 );

		//HANDLE ConsoleHandle = lStdHandle[2];
		//::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();

		if( hKernel32 != nullptr )
		{
			FreeLibrary( hKernel32 );
		}

		std::cout << "console ready.." << std::endl;
	}

	static bool initialize()
	{
		stdex_allocator_initialize();

		SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

		SERVICE_CREATE( serviceProvider, UnicodeSystem );		
		SERVICE_CREATE( serviceProvider, UnicodeService );
		SERVICE_CREATE( serviceProvider, StringizeService );
		SERVICE_CREATE( serviceProvider, ArchiveService );
		SERVICE_CREATE( serviceProvider, ArchiveService );
		SERVICE_CREATE( serviceProvider, LoggerService );
		
		class MyLoggerInterface
			: public LoggerInterface
		{
		public:
			bool initialize() override 
			{ 
				return true; 
			}
			
			void finalize() override 
			{
			};

			void setServiceProvider(ServiceProviderInterface * _serviceProvider) override 
			{ 
				m_serviceProvider = _serviceProvider; 
			}

			ServiceProviderInterface * getServiceProvider() const override 
			{ 
				return m_serviceProvider; 
			}

			void setVerboseLevel( EMessageLevel _level ) override 
			{
				(void)_level;
			};

			void setVerboseFlag( size_t _flag ) override 
			{
				(void)_flag;
			};

		public:
			bool validMessage( EMessageLevel _level, size_t _flag ) const override 
			{ 
				(void)_level;
				(void)_flag;

				return true; 
			};

		public:
			void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override
			{
				(void)_level;
				(void)_flag;
				(void)_count;

				printf("%s"
					, _data
					);
			}

			void flush() override 
			{
			}

		private:
			ServiceProviderInterface * m_serviceProvider;
		};

		LOGGER_SERVICE( serviceProvider )
			->setVerboseLevel( LM_WARNING );

		LOGGER_SERVICE( serviceProvider )
			->registerLogger( new MyLoggerInterface );

		LOGGER_WARNING(serviceProvider)("Inititalizing Config Manager..." );
		
		SERVICE_CREATE( serviceProvider, ConfigService );
		SERVICE_CREATE( serviceProvider, ConverterService );
		SERVICE_CREATE( serviceProvider, CodecService );
		SERVICE_CREATE( serviceProvider, DataService );
		SERVICE_CREATE( serviceProvider, ThreadSystem );
		SERVICE_CREATE( serviceProvider, ThreadService );
		SERVICE_CREATE( serviceProvider, MemoryService );
		SERVICE_CREATE( serviceProvider, PluginSystem );
		SERVICE_CREATE( serviceProvider, PluginService );

		SERVICE_CREATE( serviceProvider, WindowsLayer );
		SERVICE_CREATE( serviceProvider, FileService );
				
		PLUGIN_CREATE( serviceProvider, MengeWin32FileGroup );
		PLUGIN_CREATE( serviceProvider, MengeZip );
		PLUGIN_CREATE( serviceProvider, MengeLZ4 );
		PLUGIN_CREATE( serviceProvider, MengeImageCodec );

		SERVICE_CREATE( serviceProvider, LoaderService );
		
		PLUGIN_SERVICE(serviceProvider)
			->loadPlugin( L"MengeDevelopmentConverterPlugin.dll" );

		PLUGIN_SERVICE(serviceProvider)
			->loadPlugin( L"MengeXmlCodecPlugin.dll" );

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( ConstString::none(), ConstString::none(), Helper::stringizeString(serviceProvider, "dir") ) == false )
		{
			return false;
		}

		ConstString dev = Helper::stringizeString(serviceProvider, "dev");

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( dev, ConstString::none(), Helper::stringizeString(serviceProvider, "dir") ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_convert( const WString & _fromPath, const WString & _toPath, const WString & _convertType, const WString & _params )
	{		
		String utf8_fromPath;
		Helper::unicodeToUtf8( serviceProvider, _fromPath, utf8_fromPath );

		String utf8_toPath;
		Helper::unicodeToUtf8( serviceProvider, _toPath, utf8_toPath );

		String utf8_convertType;
		Helper::unicodeToUtf8( serviceProvider, _convertType, utf8_convertType );

		String utf8_params;
		Helper::unicodeToUtf8( serviceProvider, _params, utf8_params );		

		ConverterOptions options;

		options.pakName = ConstString::none();
		options.inputFileName = Helper::stringizeString(serviceProvider, utf8_fromPath);
		options.outputFileName = Helper::stringizeString(serviceProvider, utf8_toPath);
		options.params = utf8_params;

		ConverterInterfacePtr converter = CONVERTER_SERVICE(serviceProvider)
			->createConverter( Helper::stringizeString(serviceProvider, utf8_convertType) );

		if( converter == nullptr )
		{
			LOGGER_ERROR(serviceProvider)("convertPVRToHTF can't create convert '%s'\nfrom: %s\nto: %s\n"
				, utf8_convertType.c_str()
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}

		converter->setOptions( &options );

		if( converter->convert() == false )
		{
			LOGGER_ERROR(serviceProvider)( "convertPVRToHTF can't convert '%s'\nfrom: %s\nto: %s\n"
				, utf8_convertType.c_str()
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool convert( const wchar_t * fromPath, const wchar_t * toPath, const wchar_t * convertType, const wchar_t * params )
	{
		if( s_convert( fromPath, toPath, convertType, params ) == false )
		{
			LOGGER_ERROR(serviceProvider)("convert: error process %ls to %ls convert %ls"
				, fromPath
				, toPath
				, convertType
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * isAlphaInImageFile( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( serviceProvider, _path, utf8_path ) == false )
		{
			return NULL;
		}

		ConstString c_path = Helper::stringizeString(serviceProvider, utf8_path);

		InputStreamInterfacePtr stream = FILE_SERVICE(serviceProvider)
			->openInputFile( ConstString::none(), c_path, false );
		
		if( stream == nullptr )
		{
			return NULL;
		}

		const ConstString & codecType = CODEC_SERVICE(serviceProvider)
				->findCodecType( c_path );

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( codecType );

		if( imageDecoder == nullptr )
		{
			return NULL;
		}

		if( imageDecoder->prepareData( stream ) == false )
		{
			return NULL;
		}

		const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

		bool isAlpha = (dataInfo->channels == 4);

		return pybind::ret_bool( isAlpha );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * isUselessAlphaInImageFile( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( serviceProvider, _path, utf8_path ) == false )
		{
			LOGGER_ERROR(serviceProvider)("isUselessAlphaInImageFile %ls invalid unicodeToUtf8"
				, _path
				);

			return nullptr;
		}

		ConstString c_path = Helper::stringizeString(serviceProvider, utf8_path);

		Image * image = new Image( serviceProvider );

		if( image->load( c_path ) == false )
		{
			LOGGER_ERROR(serviceProvider)("isUselessAlphaInImageFile %ls invalid load"
				, _path
				);
			
			return nullptr;
		}

		if( image->getChannels() != 4 )
		{
			return pybind::ret_true();
		}

		bool useless = image->uselessalpha();

		delete image;

		return pybind::ret_bool( useless );
	}
	//////////////////////////////////////////////////////////////////////////
	Image * loadImage( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( serviceProvider, _path, utf8_path ) == false )
		{
			return nullptr;
		}

		ConstString c_path = Helper::stringizeString(serviceProvider, utf8_path);

		Image * image = new Image( serviceProvider );

		if( image->load( c_path ) == false )
		{
			return nullptr;
		}
				
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool saveImage( Image * _image, const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( serviceProvider, _path, utf8_path ) == false )
		{
			return false;
		}

		ConstString c_path = Helper::stringizeString(serviceProvider, utf8_path);

		if( _image->save( c_path ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static ColourValue color_convert( PyObject * _obj )
	{
		if( pybind::tuple_check( _obj ) == true )
		{
			if( pybind::tuple_size( _obj ) == 4 )
			{
				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				float r = pybind::extract<float>(i0);
				float g = pybind::extract<float>(i1);
				float b = pybind::extract<float>(i2);
				float a = pybind::extract<float>(i3);

				return ColourValue( a, r, g, b );
			}
			else if( pybind::tuple_size( _obj ) == 3 )
			{
				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );

				float r = pybind::extract<float>(i0);
				float g = pybind::extract<float>(i1);
				float b = pybind::extract<float>(i2);
				float a = 1.f;

				return ColourValue( a, r, g, b );
			}
		}
		else if( pybind::list_check( _obj ) == true )
		{
			if( pybind::list_size( _obj ) == 4 )
			{
				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				float r = pybind::extract<float>(i0);
				float g = pybind::extract<float>(i1);
				float b = pybind::extract<float>(i2);
				float a = pybind::extract<float>(i3);

				return ColourValue( a, r, g, b );
			}				
			else if( pybind::list_size( _obj ) == 3 )
			{
				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );


				float r = pybind::extract<float>(i0);
				float g = pybind::extract<float>(i1);
				float b = pybind::extract<float>(i2);
				float a = 1.f;

				return ColourValue( a, r, g, b );
			}
		}

		return ColourValue();
	}
	//////////////////////////////////////////////////////////////////////////
	Image * createImage( uint32_t _width, uint32_t _height, uint32_t _channel, PyObject * _colour )
	{
		Image * image = new Image( serviceProvider );

		image->create( _width, _height, _channel );
		image->fill( color_convert(_colour) );

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::WString pathSHA1( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( serviceProvider, _path, utf8_path ) == false )
		{
			return NULL;
		}

		ConstString c_path = Helper::stringizeString( serviceProvider, utf8_path );

		InputStreamInterfacePtr stream = FILE_SERVICE( serviceProvider )
			->openInputFile( ConstString::none(), c_path, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR( serviceProvider )("pathSHA1 %ls invalid open '%s'"
				, _path
				, c_path
				);

			return L"";
		}

		size_t size = stream->size();

		uint8_t * buf = new uint8_t[size];

		stream->read( buf, size );

		unsigned char hash[20];
		stdex::sha1_calc( buf, size, hash );

		delete[] buf;

		char hex[41];
		stdex::sha1_hex( hash, hex );

		return Menge::WString( hex, hex + 40 );
	}
	//////////////////////////////////////////////////////////////////////////
	class PythonLogger
	{
	public:
		PythonLogger( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider(_serviceProvider)
			, m_softspace(0)
		{
		}

		~PythonLogger()
		{
		}

	public:
		void write( const char * _msg, size_t _size )
		{
			LOGGER_ERROR(m_serviceProvider).logMessage( _msg, _size );
		}

	public:
		PyObject * py_write( PyObject * _args, PyObject * _kwds )
		{
			(void)_kwds;

			if( pybind::tuple_check( _args ) == false )
			{
				return pybind::ret_none();
			}

			size_t tuple_size = pybind::tuple_size(_args);

			if( tuple_size == 0 )
			{
				return pybind::ret_none();
			}

			PyObject * arg = pybind::tuple_getitem(_args, 0);

			if( pybind::string_check( arg ) == true )
			{
				size_t size;
				const char * str = pybind::string_to_char_and_size( arg, size );

				this->write( str, size );
			}
			else if( pybind::unicode_check( arg ) == true )
			{
				size_t size;
				const char * utf8 = pybind::unicode_to_utf8_and_size( arg, size );

				this->write( utf8, size );
			}

			return pybind::ret_none();
		}

		PyObject * py_flush( PyObject * _args, PyObject * _kwds )
		{
			(void)_args;
			(void)_kwds;

			return pybind::ret_none();
		}

	public:
		void setSoftspace( int _softspace )
		{
			m_softspace = _softspace;
		}

		int getSoftspace() const
		{
			return m_softspace;
		}

		const wchar_t * getErrors() const
		{
			return L"strict";
		}

		void setErrors( const wchar_t * _errors )
		{
			printf("setErrors %ls\n"
				, _errors
				);
		}

		const wchar_t * getEncoding() const
		{
			return L"ascii";
		}

		void setEncoding( const wchar_t * _encoding ) const
		{
			printf("setEncoding %ls\n"
				, _encoding
				);
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		int m_softspace;
	};
}
//////////////////////////////////////////////////////////////////////////
static void s_error( const wchar_t * _msg )
{
	LOGGER_ERROR(Menge::serviceProvider)("%ls"
		, _msg
		);
}
//////////////////////////////////////////////////////////////////////////
static bool getCurrentUserRegValue( const WCHAR * _path, WCHAR * _value, DWORD _size )
{
	HKEY hKey;
	LONG lRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path, 0, KEY_READ, &hKey );  // Check Python x32
	if( lRes == ERROR_FILE_NOT_FOUND )
	{
		lRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );  // Check Python x64
	}

	if( lRes != ERROR_SUCCESS )
	{
		LOGGER_ERROR( Menge::serviceProvider )("getCurrentUserRegValue %ls RegOpenKeyEx get Error %d"
			, _path
			, lRes
			);

		return false;
	}

	DWORD dwBufferSize = _size;
	LONG nError = ::RegQueryValueEx( hKey, L"", 0, NULL, (LPBYTE)_value, &dwBufferSize );

	RegCloseKey( hKey );

	if( nError != ERROR_SUCCESS )
	{
		LOGGER_ERROR( Menge::serviceProvider )("getCurrentUserRegValue %ls RegQueryValueEx get Error %d"
			, _path
			, nError
			);

		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool run()
{
	try
	{
		if( Menge::initialize() == false )
		{
			printf("PyInit_ToolsBuilderPlugin Menge::initialize failed\n"
				);

			return false;
		}
	}
	catch( const std::exception & se )
	{
		char MSG[1024];
		sprintf(MSG, "PyInit_ToolsBuilderPlugin exception %s"			
			, se.what()			
			);

		MessageBoxA(NULL, MSG, "Error", MB_OK);

		return false;
	}

	LOGGER_ERROR(Menge::serviceProvider)("Menge::initialize complete");

	const WCHAR * szRegPath = L"SOFTWARE\\Python\\PythonCore\\3.5-32\\PythonPath";

	{
		WCHAR szPythonPath[512];
		if( getCurrentUserRegValue( szRegPath, szPythonPath, 512 ) == false )
		{
			LOGGER_ERROR( Menge::serviceProvider )("invalid get reg value '%ls'"
				, szRegPath
				);
		}

		pybind::set_path( szPythonPath );

		LOGGER_ERROR(Menge::serviceProvider)("set python path '%ls'"
			, szPythonPath
			);
	}	

	if( pybind::initialize( false, false, false ) == false )
	{
		return false;
	}

	LOGGER_ERROR(Menge::serviceProvider)("pybind::initialize complete");

	PyObject * py_tools_module = pybind::module_init( "ToolsBuilderPlugin" );

	pybind::interface_<Menge::PythonLogger>("XlsScriptLogger", true, py_tools_module)
		.def_native("write", &Menge::PythonLogger::py_write )
		.def_native("flush", &Menge::PythonLogger::py_flush )
		.def_property("softspace", &Menge::PythonLogger::getSoftspace, &Menge::PythonLogger::setSoftspace )
		.def_property("errors", &Menge::PythonLogger::getErrors, &Menge::PythonLogger::setErrors )
		.def_property("encoding", &Menge::PythonLogger::getEncoding, &Menge::PythonLogger::setEncoding )
		;
	Menge::PythonLogger * logger = new Menge::PythonLogger(Menge::serviceProvider);
	PyObject * py_logger = pybind::ptr(logger);

	pybind::setStdOutHandle( py_logger );
	pybind::setStdErrorHandle( py_logger );

	pybind::def_function( "spreadingPngAlpha", &Menge::spreadingPngAlpha, py_tools_module );
	pybind::def_function( "writeBin", &Menge::writeBin, py_tools_module );
	pybind::def_function( "writeAek", &Menge::writeAek, py_tools_module );

	pybind::def_function( "convert", &Menge::convert, py_tools_module );	
	pybind::def_function( "isAlphaInImageFile", &Menge::isAlphaInImageFile, py_tools_module );
	pybind::def_function( "isUselessAlphaInImageFile", &Menge::isUselessAlphaInImageFile, py_tools_module );

	Menge::Image::embedding( py_tools_module );

	pybind::def_function( "loadImage", &Menge::loadImage, py_tools_module );
	pybind::def_function( "saveImage", &Menge::saveImage, py_tools_module );
	pybind::def_function( "createImage", &Menge::createImage, py_tools_module );

	pybind::def_function( "pathSHA1", &Menge::pathSHA1, py_tools_module );

	PyObject * module_builtins = pybind::get_builtins();

	pybind::def_function( "Error", &s_error, module_builtins );

	pybind::incref( py_tools_module );
	pybind::module_addobject( module_builtins, "ToolsBuilderPlugin", py_tools_module );

	Menge::WChar currentDirectory[MAX_PATH];

	if( ::GetCurrentDirectory( MAX_PATH, currentDirectory ) == 0 )
	{
		return false;
	}

	PyObject * py_syspath = pybind::list_new( 0 );

	{
		WCHAR szPythonPath[512];
		if( getCurrentUserRegValue( szRegPath, szPythonPath, 512 ) == false )
		{
			LOGGER_ERROR( Menge::serviceProvider )("invalid get reg value '%ls'"
				, szRegPath
				);
		}

		WCHAR * ch = wcstok( szPythonPath, L";" );

		while( ch != NULL )
		{
			PyObject * py_stdpath = pybind::ptr( ch );
			pybind::list_appenditem( py_syspath, py_stdpath );
			pybind::decref( py_stdpath );

			ch = wcstok( NULL, L";");
		}
	}

	PyObject * py_currentpath = pybind::ptr( currentDirectory );
	pybind::list_appenditem( py_syspath, py_currentpath );
	pybind::decref( py_currentpath );

	pybind::set_syspath( py_syspath );

	pybind::decref( py_syspath );

	LPWSTR lpwCmdLine = GetCommandLineW();


	int nArgs = 0;
	LPWSTR * szArglist = CommandLineToArgvW( lpwCmdLine, &nArgs );

	if( nArgs < 3 )
	{
		return false;
	}

	PWSTR szModuleName = szArglist[1];

	Menge::String utf8_ModuleName;
	Menge::Helper::unicodeToUtf8( Menge::serviceProvider, szModuleName, utf8_ModuleName );

	PWSTR szFunctionName = szArglist[2];

	Menge::String utf8_FunctionName;
	Menge::Helper::unicodeToUtf8( Menge::serviceProvider, szFunctionName, utf8_FunctionName );

	LOGGER_ERROR( Menge::serviceProvider )("Module '%s' Function '%s'"
		, utf8_ModuleName.c_str()
		, utf8_FunctionName.c_str()
		);

	bool exist_run;
	PyObject * py_run_module = py_run_module = pybind::module_import( utf8_ModuleName.c_str(), exist_run );

	if( py_run_module == nullptr )
	{
		return false;
	}

	PyObject * py_args = pybind::tuple_new( nArgs - 3 );

	for( int i = 3; i != nArgs; ++i )
	{
		LPWSTR arg = szArglist[i];
		
		pybind::tuple_setitem_t( py_args, i - 3, arg );
	}

	pybind::call_method_native( py_run_module, utf8_FunctionName.c_str(), py_args );

	pybind::decref( py_args );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////
int CALLBACK WinMain( _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow )
{ 
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;

	Menge::createConsole();

	run();
	
	return 0;
}