#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/RenderSystemInterface.h"

#   include "Factory/FactorablePtr.h"
#	include "Core/ConstString.h"
#	include "Core/Params.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	class ModuleInterface
		: public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

	public:
		virtual void setName( const ConstString & _name ) = 0;
		virtual const ConstString & getName() const = 0;

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual void update( bool _focus ) = 0;

	public:
		virtual void tick( float _time, float _timing ) = 0;
		virtual void render( const RenderObjectState * _state, uint32_t _debugMask ) = 0;

	public:
		virtual void message( const ConstString & _messageName, const TMapParams & _params ) = 0;
		virtual void messageAll( const ConstString & _messageName, const TMapParams & _params ) = 0;
	};

	typedef stdex::intrusive_ptr<ModuleInterface> ModuleInterfacePtr;

	class ModuleFactoryInterface
		: public FactorablePtr
	{
	public:
		virtual ModuleInterfacePtr createModule() = 0;
	};

	typedef stdex::intrusive_ptr<ModuleFactoryInterface> ModuleFactoryInterfacePtr;

	class ModuleServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("ModuleService")

	public:
		virtual bool registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory ) = 0;
		virtual void unregisterModule( const ConstString & _name ) = 0;

	public:
		virtual bool runModule( const ConstString & _name ) = 0;
		virtual void stopModule( const ConstString & _name ) = 0;

	public:
		virtual void stopModules() = 0;

	public:
		virtual void update( bool _focus ) = 0;

	public:
		virtual void tick( float _time, float _timing ) = 0;
		virtual void render( const RenderObjectState * _state, unsigned int _debugMask ) = 0;

	public:
		virtual void message( const ConstString & _moduleName, const ConstString & _messageName, const TMapParams & _params ) = 0;
		virtual void messageAll( const ConstString & _messageName, const TMapParams & _params ) = 0;
	};

#   define MODULE_SERVICE( serviceProvider )\
	((Menge::ModuleServiceInterface *)SERVICE_GET(serviceProvider, Menge::ModuleServiceInterface))
}