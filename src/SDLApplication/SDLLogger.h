#   pragma once

#   include "Interface/LoggerInterface.h"

namespace Menge
{
    class SDLLogger
        : public LoggerInterface
    {
    public:
        SDLLogger();
        virtual ~SDLLogger();

    public:
        void setServiceProvider(ServiceProviderInterface * _serviceProvider) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

        bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

    public:
        void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;
        void flush() override;

    protected:
        EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;
        ServiceProviderInterface * m_serviceProvider;
        bool m_initialize;
    };
}