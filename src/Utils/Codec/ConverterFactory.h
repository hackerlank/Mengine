#   pragma once

#   include "Interface/ConverterInterface.h"

#   include "Core/ConstString.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
    template<class T>
    class ConverterFactory
        : public ConverterFactoryInterface
    {
    public:
        ConverterFactory( ServiceProviderInterface * _serviceProvider, const ConstString & _name )
            : m_serviceProvider(_serviceProvider)
            , m_name(_name)
        {
        }

        ~ConverterFactory()
        {
        }

    protected:
        ConverterInterfacePtr createConverter() override
        {
			ConverterInterface * converter = m_factory.createObject();

			converter->setServiceProvider( m_serviceProvider );

            return converter;
        }

        const ConstString & getName() const override
        {
            return m_name;
        }

    protected:
        void destroy() override
        {
            delete this;
        }

    protected:
        ServiceProviderInterface * m_serviceProvider;		
        ConstString m_name;

		typedef FactoryPoolStore<T, 8> TFactoryConverter;
		TFactoryConverter m_factory;
    };
}