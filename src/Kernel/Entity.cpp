#	include "Entity.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Entity::Entity()
		: m_scriptEventable(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Entity::~Entity()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::setPrototype( const ConstString & _prototype )
	{
		m_prototype = _prototype;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Entity::getPrototype() const
	{
		return m_prototype;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::setScriptEventable( Eventable * _eventable )
	{
		m_scriptEventable = _eventable;
	}
	//////////////////////////////////////////////////////////////////////////
	Eventable * Entity::getScriptEventable() const
	{
		return m_scriptEventable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::setScriptObject( const pybind::object & _object )
	{
		m_object = _object;
	}
	//////////////////////////////////////////////////////////////////////////
	const pybind::object & Entity::getScriptObject() const
	{
		return m_object;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_PREPARATION, EntityEventReceiver )
            ->onEntityPreparation( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_PREPARATION )(m_object);

		bool successful = Node::_activate();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_ACTIVATE, EntityEventReceiver )
            ->onEntityActivate(m_object);
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_ACTIVATE )(m_object);
	}
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_PREPARATION_DEACTIVATE, EntityEventReceiver )
            ->onEntityPreparationDeactivate( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_PREPARATION_DEACTIVATE )(m_object);

        Node::_deactivate();		
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_DEACTIVATE, EntityEventReceiver )
            ->onEntityDeactivate( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_DEACTIVATE )(m_object);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_COMPILE, EntityEventReceiver )
            ->onEntityCompile( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_COMPILE )(m_object);
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_RELEASE, EntityEventReceiver )
            ->onEntityRelease( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_RELEASE )(m_object);
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCreate()
	{
        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_CREATE, EntityEventReceiver )
            ->onEntityCreate( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_CREATE )(m_object, this);
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::destroy()
	{
        if( this->isShallowGrave() == true )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( this );

			this->release();

			this->setShallowGravePropagate( true );
			
            return;
        }

        Factorable::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_destroy()
	{
		this->release();

		Node * old_parent = this->getParent();

        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_DESTROY, EntityEventReceiver )
            ->onEntityDestroy( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_DESTROY )(m_object);

		m_object.reset();

		Node * new_parent = this->getParent();

		if( old_parent != new_parent )
		{
			LOGGER_ERROR( m_serviceProvider )("Entity::destroy %s:%s script event EVENT_DESTROY replace node to other hierarchy"
				, this->getType().c_str()
				, this->getName().c_str()
				);

			return;
		}

		this->destroyAllChild();
		this->removeFromParent();

		this->unwrap();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_unshallowGrave()
	{
		this->setShallowGravePropagate( false );

		this->removeFromParent();

        EVENTABLE_METHODT( m_scriptEventable, EVENT_ENTITY_DESTROY, EntityEventReceiver )
            ->onEntityDestroy( m_object );
		//EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ENTITY_DESTROY )(m_object);

        m_object.reset();

		Factorable::destroy();
	}
}