#	pragma once

#	include "Codec/SoundDecoder.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class SoundDecoderWAV
		: public SoundDecoder
	{
	public:
		SoundDecoderWAV();
		~SoundDecoderWAV();

	public:
		bool _prepareData() override;

	public:
		size_t _decode( void * _buffer, size_t _bufferSize ) override;

		bool _seek( float _timing ) override;
		float _tell() const override;

	protected:		
		bool findChunkData_( size_t & _size );

	protected:
		uint32_t m_chunkDataPos;
	};
}	// namespace Menge
