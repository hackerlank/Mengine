#	include "Box2DJoint.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint::Box2DJoint()
		: m_world(nullptr)
		, m_joint(nullptr)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	Box2DJoint::~Box2DJoint()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Box2DJoint::initialize( b2World* _world, const b2JointDef * _jointDef )
	{
		m_world = _world;
		
		b2Joint* joint = m_world->CreateJoint( _jointDef );

		if( joint == nullptr )
		{
			return false;
		}

		m_joint = joint;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	b2World* Box2DJoint::getWorld() const
	{
		return m_world;
	}
    //////////////////////////////////////////////////////////////////////////
	b2Joint* Box2DJoint::getJoint() const
    {
        return m_joint;
    }
    //////////////////////////////////////////////////////////////////////////
}