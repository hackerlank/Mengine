/*
 *	OALSoundBufferBase.cpp
 *
 *	Created by _Berserk_ on 24.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundBufferBase.h"
#	include "Interface/SoundCodecInterface.h"

#	define OAL_CHECK_ERROR() s_OALErrorCheck( m_soundSystem, __FILE__, __LINE__ )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::OALSoundBufferBase()
        : m_format(0)
        , m_frequency(0)
        , m_channels(0)
        , m_time_total(0.f)
        , m_isStereo(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::~OALSoundBufferBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		// determine data parameters
		const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->time_total_secs;
		if( m_channels == 1 )
		{
			m_format = AL_FORMAT_MONO16;
			m_isStereo = false;
		}
		else
		{
			m_format = AL_FORMAT_STEREO16;
			m_isStereo = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::play( ALenum _source, bool _looped, float _pos )
	{
        (void)_source;
        (void)_looped;
        (void)_pos;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::pause( ALenum _source )
	{
        (void)_source;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::stop( ALenum _source )
	{
        (void)_source;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::getTimePos( ALenum _source, float & _pos ) const
	{
        (void)_source;
        (void)_pos;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::isStereo() const
	{
		return m_isStereo;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBufferBase::getTimeTotal() const
	{
		return m_time_total;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::update()
	{
		// do nothing
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
