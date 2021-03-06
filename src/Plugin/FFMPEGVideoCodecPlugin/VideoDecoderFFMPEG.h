#	pragma once

#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif

#	include "Codec/VideoDecoder.h"

#pragma warning(push, 0) 
extern "C"
{
	#	include "libavformat/avformat.h"
	#	include "libswscale/swscale.h"
	#	include "libavcodec/avcodec.h"
	#	include "libavutil/mathematics.h"
}
#pragma warning(pop)

namespace Menge
{
	class VideoDecoderFFMPEG
		: public VideoDecoder
	{
	public:
		VideoDecoderFFMPEG();
		~VideoDecoderFFMPEG();

	public:
		bool _prepareData() override;

	public:
		bool _rewind() override;

    public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	
	public:	
		float getTiming()  const override;
		bool seek( float _timing ) override;


    public:
        void setPitch( size_t _pitch ) override;

	public:		
		EVideoDecoderReadState readNextFrame( float & _pts ) override;

    protected:
        bool checkVideoCorrect_();
	
	protected:
		AVFormatContext * m_formatContext;
		AVCodecContext * m_codecContext;
				
		AVFrame * m_frame; 		
		AVIOContext * m_IOContext;

		SwsContext * m_imgConvertContext;
        bool m_imgConvertContextCache;
		
		int m_outputPixelFormat;

		uint8_t m_bufferIO[FF_MIN_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
		int m_videoStreamId;
		
		//float m_timing;
		float m_pts;

        size_t m_pitch;
		
	private:
		void clear_();
	};
}	// namespace Menge
