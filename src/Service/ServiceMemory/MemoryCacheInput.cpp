#	include "MemoryCacheInput.h"

#	include "MemoryManager.h"

#	include "Interface/MemoryInterface.h"

#	include "stdex/memorycopy.h"

//#	include <memory.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInput::MemoryCacheInput()
		: m_serviceProvider(nullptr)
		, m_memoryManager(nullptr)
		, m_bufferId(0)
		, m_data(nullptr)
		, m_size(0)
		, m_pos(nullptr)
		, m_end(nullptr)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInput::~MemoryCacheInput()
	{
		this->uncache_();
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheInput::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheInput::setMemoryManager( MemoryManager * _memoryManager )
	{ 
		m_memoryManager = _memoryManager;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheInput::uncache_()
	{
		if( m_bufferId != 0 )
		{
			m_memoryManager->unlockBuffer( m_bufferId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer MemoryCacheInput::cacheMemory( size_t _size, const char * _doc )
	{
		this->uncache_();

		void * memory;
		uint32_t bufferId = m_memoryManager->lockBuffer( _size, &memory, _doc );

		if( bufferId == INVALID_CACHE_BUFFER_ID )
		{
			return nullptr;
		}

		m_bufferId = bufferId;

		m_data = reinterpret_cast<unsigned char *>(memory);
		m_size = _size;

		m_pos = m_data;
		m_end = m_data + m_size;

        return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer MemoryCacheInput::getMemory() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::read( void * _buf, size_t _size )
	{
		STDEX_THREAD_GUARD_SCOPE( this, "MemoryCacheInput::read" );

		size_t cnt = _size;
		// Read over end of memory?
		if( m_pos + cnt > m_end )
		{
			cnt = m_end - m_pos;
		}

		if( cnt == 0 )
		{
			return 0;
		}

		stdex::memorycopy( _buf, 0, m_pos, cnt );
		//std::copy( m_pos, m_pos + cnt, (unsigned char *)_buf );

		m_pos += cnt;

		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::seek( size_t _pos )
	{
		STDEX_THREAD_GUARD_SCOPE( this, "MemoryCacheInput::seek" );

		if( _pos > m_size )
		{
			_pos = m_size;
		}

		m_pos = m_data + _pos;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::skip( size_t _pos )
	{
		STDEX_THREAD_GUARD_SCOPE( this, "MemoryCacheInput::skip" );

		if( m_pos + _pos > m_end )
		{
			_pos = 0;
		}

		m_pos += _pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::eof() const
	{
		STDEX_THREAD_GUARD_SCOPE( this, "MemoryCacheInput::eof" );

		return m_pos == m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::tell() const
	{
		STDEX_THREAD_GUARD_SCOPE( this, "MemoryCacheInput::tell" );

        size_t distance = m_pos - m_data;

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::time( uint64_t & _time ) const
	{
        (void)_time;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::memory( void ** _memory, size_t * _size )
	{
		*_memory = m_data;
		*_size = m_size;

		return true;
	}
}	// namespace Menge
