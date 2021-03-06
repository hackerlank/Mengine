#	include "MDLOptions.h"

#	include "stdex/memory_reader.h"
#	include "stdex/memory_writer.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	const uint32_t mdl_magic = 0xA005BACE;
	const uint32_t mdl_version = 1;
	//////////////////////////////////////////////////////////////////////////
	MDLOptions::MDLOptions()
		: width(0)
		, height(0)
		, model(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MDLOptions::readFlatten( const unsigned char * _flatten )
	{
		size_t read = 0;
		stdex::memory_reader mr( _flatten, MDL_IO_FLATTEN_OPTIONS_SIZE, read );

		try
		{
			uint32_t magic;
			mr.readPOD( magic );

			if( magic != mdl_magic )
			{
				return false;
			}

			uint32_t version;
			mr.readPOD( version );

			if( version != mdl_version )
			{
				return false;
			}

			//wchar_t path_buff[AEGP_MAX_PATH_SIZE];
			//mr.readCount( path_buff, AEGP_MAX_PATH_SIZE );			
			//path.assign( path_buff );

			uint32_t time;
			mr.readPOD( time );

			duration.value = (A_long)time;
			duration.scale = 1000;

			mr.readPOD( width );
			mr.readPOD( height );
		}
		catch( const stdex::memory_reader_exception & )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MDLOptions::writeFlatten( unsigned char * _flatten )
	{
		size_t write = 0;
		stdex::memory_writer mw( _flatten, MDL_IO_FLATTEN_OPTIONS_SIZE, write );

		try
		{
			mw.writePOD( mdl_magic );
			mw.writePOD( mdl_version );

			//wchar_t path_buff[AEGP_MAX_PATH_SIZE];
			//wcscpy_s( path_buff, path.c_str() );

			//mw.writeCount( path_buff, AEGP_MAX_PATH_SIZE );

			uint32_t time = (uint32_t)duration.value;
			mw.writePOD( time );

			mw.writePOD( width );
			mw.writePOD( height );
		}
		catch( stdex::memory_writer_exception & )
		{
			return false;
		}

		return true;
	}
}