#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Kernel/Updatable.h"

#	include "Factory/FactorablePtr.h"
#	include "Core/ConstString.h"
#	include "Math/vec3.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	class Node;

	class Motor
		: public FactorablePtr
		, public Updatable
	{
	public:
		Motor();
		~Motor();

	public:
		void setServiceProvider(ServiceProviderInterface * m_serviceProvider);
		ServiceProviderInterface * getServiceProvider() const;

	public:
		void setNode(Node * _node);
		Node * getNode() const;

	public:
		void addVelocity(const ConstString & _name, const mt::vec3f & _velocity);
		bool hasVelocity(const ConstString & _name) const;
		mt::vec3f getVelocity(const ConstString & _name) const;

	protected:
		void _update(float _current, float _timing) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Node * m_node;

		struct VelocityDesc
		{
			mt::vec3f velocity;
		};

		typedef stdex::map<ConstString, VelocityDesc> TMapVelocity;
		TMapVelocity m_velocities;
	};
}