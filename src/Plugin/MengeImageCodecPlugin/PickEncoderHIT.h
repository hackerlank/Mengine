#	pragma once

#   include "Interface/ArchiveInterface.h"

#	include "Codec/PickEncoder.h"

namespace Menge
{
	class PickEncoderHIT
		: public PickEncoder
	{
	public:
		PickEncoderHIT();
		~PickEncoderHIT();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, size_t _size, const CodecDataInfo* _bufferDataInfo ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;
	};
}	// namespace Menge
