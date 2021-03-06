#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	const uint32_t SERVICE_PROVIDER_NAME_SIZE = 64;
    const uint32_t SERVICE_PROVIDER_COUNT = 64;

	class ServiceProvider
		: public ServiceProviderInterface
	{
    public:
        ServiceProvider();
        ~ServiceProvider();

	public:
		bool existService( const char * _name ) const override;

    protected:
        ServiceInterface * getService( const char * _name ) const override;

	protected:
        ServiceInterface * generateService( TServiceProviderGenerator _generator ) override;
		bool initializeService( TServiceProviderGenerator _generator ) override;
		bool finalizeService( const char * _name ) override;

		bool destroyService( const char * _name ) override;
		
		//bool registryService( const char * _name, ServiceInterface * _service ) override;
		//bool unregistryService( const char * _name ) override;
        
	protected:
		void destroy() override;

	protected:
        struct ServiceDesc
        {
            char name[SERVICE_PROVIDER_NAME_SIZE];
            ServiceInterface * service;
        };

        ServiceDesc m_services[SERVICE_PROVIDER_COUNT];
	};
}