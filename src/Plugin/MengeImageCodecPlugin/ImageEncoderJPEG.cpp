#	include "ImageEncoderJPEG.h"

#	include "Interface/FileSystemInterface.h"

#	include "Utils/Logger/Logger.h"

#	define OUTPUT_BUF_SIZE 4096				// choose an efficiently fwrite'able size

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct DestinationManager 
	{
		/// public fields
		struct jpeg_destination_mgr pub;

		OutputStreamInterface * m_stream;
		/// start of buffer
		unsigned char * buffer;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef DestinationManager * menge_dst_ptr;
	//////////////////////////////////////////////////////////////////////////
	struct EncoderJPEGErrorManager
	{
		jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;
	};
	//////////////////////////////////////////////////////////////////////////
	static void	s_jpegErrorExit( j_common_ptr _cinfo ) 
	{
		EncoderJPEGErrorManager * mErr = (EncoderJPEGErrorManager *)_cinfo->err;
		// always display the message
		char buffer[JMSG_LENGTH_MAX];

		// create the message
		(*mErr->pub.format_message)(_cinfo, buffer);
		// send it to user's message proc
		//FreeImage_OutputMessageProc(s_format_id, buffer);

		ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface*>(_cinfo->client_data);

		LOGGER_ERROR(serviceProvider)("s_jpegErrorExit %s"
			, buffer
			);

		// allow JPEG with a premature end of file
		if( mErr->pub.msg_parm.i[0] != 13 )
		{
			// let the memory manager delete any temp files before we die
			jpeg_destroy( _cinfo );

			// Return control to the setjmp point
			longjmp( mErr->setjmp_buffer, 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// Actual output of any JPEG message.  Note that this method does not know
	// how to generate a message, only where to send it.
	static void	s_jpegOutputMessage( j_common_ptr _cinfo ) 
	{
		EncoderJPEGErrorManager * mErr = (EncoderJPEGErrorManager *)_cinfo->err;

		char buffer[JMSG_LENGTH_MAX];

		// create the message
		(*mErr->pub.format_message)(_cinfo, buffer);
		// send it to user's message proc
		//FreeImage_OutputMessageProc(s_format_id, buffer);

        ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface*>(_cinfo->client_data);

		LOGGER_ERROR(serviceProvider)("s_jpegOutputMessage %s"
            , buffer
            );
	}

	//Initialize destination.  This is called by jpeg_start_compress()
	//before any data is actually written. It must initialize
	//next_output_byte and free_in_buffer. free_in_buffer must be
	//initialized to a positive value.

	METHODDEF(void)
		init_destination (j_compress_ptr cinfo) 
	{
		menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

		dest->buffer = (unsigned char *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
			OUTPUT_BUF_SIZE * sizeof(unsigned char));

		dest->pub.next_output_byte = dest->buffer;
		dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
	}

	//This is called whenever the buffer has filled (free_in_buffer
	//reaches zero). In typical applications, it should write out the
	//*entire* buffer (use the saved start address and buffer length;
	//ignore the current state of next_output_byte and free_in_buffer).
	//Then reset the pointer & count to the start of the buffer, and
	//return TRUE indicating that the buffer has been dumped.
	//free_in_buffer must be set to a positive value when TRUE is
	//returned.  A FALSE return should only be used when I/O suspension is
	//desired.
	METHODDEF(boolean)
		empty_output_buffer (j_compress_ptr cinfo) 
	{
		menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

		dest->m_stream->write( dest->buffer, OUTPUT_BUF_SIZE );

		dest->pub.next_output_byte = dest->buffer;
		dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

		return TRUE;
	}

	//Terminate destination --- called by jpeg_finish_compress() after all
	//data has been written.  In most applications, this must flush any
	//data remaining in the buffer.  Use either next_output_byte or
	//free_in_buffer to determine how much data is in the buffer.
	METHODDEF(void)
		term_destination (j_compress_ptr cinfo) 
	{
		menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

		size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

		// write any data remaining in the buffer

		if (datacount > 0) 
		{
			dest->m_stream->write(dest->buffer, (unsigned int)datacount );
		}
	}


	//Prepare for output to a stdio stream.
	//The caller must have already opened the stream, and is responsible
	//for closing it after finishing compression.
	GLOBAL(void) jpeg_menge_dst(j_compress_ptr cinfo, OutputStreamInterface * _stream ) 
	{
		menge_dst_ptr dest;

		if( cinfo->dest == nullptr )
		{
			cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)
				((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(DestinationManager));
		}

		dest = (menge_dst_ptr) cinfo->dest;
		dest->pub.init_destination = init_destination;
		dest->pub.empty_output_buffer = empty_output_buffer;
		dest->pub.term_destination = term_destination;
		dest->m_stream = _stream;
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderJPEG::ImageEncoderJPEG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderJPEG::~ImageEncoderJPEG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageEncoderJPEG::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageEncoderJPEG::encode( const void * _buffer, size_t _size, const CodecDataInfo* _dataInfo )
	{
		(void)_size;

		const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( _dataInfo );

		EncoderJPEGErrorManager errorMgr;

		errorMgr.pub.error_exit = &s_jpegErrorExit;
		errorMgr.pub.output_message = &s_jpegOutputMessage;

		if( setjmp( errorMgr.setjmp_buffer ) )
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.
			
			return 0;
		}
		
		struct jpeg_compress_struct cinfo = {0};
		cinfo.err = jpeg_std_error(&errorMgr.pub);
		cinfo.client_data = m_serviceProvider;

		jpeg_create_compress( &cinfo );

		jpeg_menge_dst( &cinfo, m_stream.get() );

		cinfo.image_width = (JDIMENSION)dataInfo->width;
		cinfo.image_height = (JDIMENSION)dataInfo->height;
		cinfo.input_components = dataInfo->channels;

		cinfo.in_color_space = JCS_RGB;
				
		//int pixel_depth = 8;

		jpeg_set_defaults( &cinfo );
		jpeg_set_quality( &cinfo, dataInfo->quality, TRUE );

		jpeg_start_compress( &cinfo, TRUE );

        JSAMPROW jpeg_buffer = (JSAMPROW)_buffer;

		size_t pitch = m_options.pitch;

		JSAMPROW row_pointer[1];	// pointer to JSAMPLE row[s]
		while (cinfo.next_scanline < cinfo.image_height) 
		{
			// jpeg_write_scanlines expects an array of pointers to scanlines.
			// Here the array is only one element long, but you could pass
			// more than one scanline at a time if that's more convenient.
			row_pointer[0] = jpeg_buffer + cinfo.next_scanline * pitch;
			(void) jpeg_write_scanlines( &cinfo, row_pointer, 1);
		}

		// Step 6: Finish compression

		jpeg_finish_compress( &cinfo );
		jpeg_destroy_compress( &cinfo );

		return pitch * dataInfo->height;
	}
}	// namespace Menge
