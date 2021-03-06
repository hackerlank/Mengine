#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "stdex/thread_guard.h"

// fwd-declaration
struct SDL_RWops;

namespace Menge
{
    class SDLFileInputStream
        : public FileInputStreamInterface
    {
    public:
        SDLFileInputStream();
        ~SDLFileInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        bool open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size ) override;
        
    public:
        size_t read( void * _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t & _time ) const override;

    public:
        bool memory( void ** _memory, size_t * _size ) override;

    protected:
        bool openFile_( const FilePath & _folder, const FilePath & _fileName, Char * _filePath );
        void close_();
        bool read_( void * _buf, size_t _size, size_t & _read );
        bool seek_( size_t _pos );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        SDL_RWops* m_rwops;

        size_t m_size;
        size_t m_offset;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;

        uint8_t m_readCache[MENGINE_FILE_STREAM_BUFFER_SIZE];

        STDEX_THREAD_GUARD_INIT;

#	ifdef _DEBUG
        String m_folder;
        String m_fileName;
#	endif
    };
}	// namespace Menge
