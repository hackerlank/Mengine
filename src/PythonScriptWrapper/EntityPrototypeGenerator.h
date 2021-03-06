#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Eventable.h"
#	include "Kernel/Entity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class EntityPrototypeGenerator
		: public PrototypeGeneratorInterface
		, public Eventable
	{
	public:
		EntityPrototypeGenerator();
		~EntityPrototypeGenerator();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		void setScriptGenerator( const pybind::object & _generator );

	public:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		pybind::object preparePythonType();

	protected:
		Factorable * generate() override;

	public:
		PyObject * getGenerator() const;

	protected:
		uint32_t count() const override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_category;
		ConstString m_prototype;
		pybind::object m_generator;
		pybind::object m_type;

		uint32_t m_count;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
	
}