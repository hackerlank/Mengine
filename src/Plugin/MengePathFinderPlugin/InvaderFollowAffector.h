#	pragma once

#	include "Kernel/Affector.h"
#	include "Kernel/Node.h"

#	include "Math/vec2.h"

#	include "Core/ValueFollower.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class InvaderFollowAffector
		: public Affector
		, public pybind::bindable
	{
	public:
		InvaderFollowAffector();
		~InvaderFollowAffector();

	public:
		Node * getNode() const;
		Node * getTarget() const;

	public:
		void setMoveSpeed( float _moveSpeed );
		float getMoveSpeed() const;
	
	public:
		bool initialize( Node * _node, Node * _target, const pybind::object & _cb, const pybind::detail::args_operator_t & _args );

	protected:
		bool _affect( float _timing ) override;
		void complete() override;
		void stop() override;		

	protected:
		PyObject * _embedded() override;

	protected:
		Node * m_node;
		Node * m_target;

		float m_moveSpeed;

		pybind::object m_cb;
		pybind::detail::args_operator_t m_args;
	};
}

