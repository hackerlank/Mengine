#   pragma once

#   include "Interface/ThreadSystemInterface.h"

#	include "SDL_Thread.h"

namespace Menge
{
    class SDLThreadMutex
        : public ThreadMutexInterface
    {
    public:
        SDLThreadMutex();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, const char * _doc);

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        SDL_mutex * m_cs;
        const char * m_doc;
    };

    typedef stdex::intrusive_ptr<SDLThreadMutex> SDLThreadMutexPtr;
}