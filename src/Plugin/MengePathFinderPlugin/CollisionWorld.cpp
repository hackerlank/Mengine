#	include "CollisionWorld.h"

#	include "Math/capsule2.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FActorDead
		{
			bool operator ()( const CollisionActorPtr & _actor ) const
			{
				return _actor->isRemoved();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionWorld::CollisionWorld()
		: m_remove(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionWorld::~CollisionWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::initialize()
	{
		for( uint32_t i = 0; i != 8; ++i )
		{
			for( uint32_t j = 0; j != 8; ++j )
			{
				m_iffs[i][j] = false;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::setIFFs( uint32_t _iff1, uint32_t _iff2, bool _value )
	{
		m_iffs[_iff1][_iff2] = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionWorld::getIFFs( uint32_t _iff1, uint32_t _iff2 ) const
	{
		return m_iffs[_iff1][_iff2];
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionActorPtr CollisionWorld::createActor( CollisionActorProviderInterface * _provider, float _radius, uint32_t _iff, bool _active )
	{
		CollisionActorPtr actor = m_factoryCollisionActor.createObject();

		actor->setCollisionActorProvider( _provider );
		actor->setRadius( _radius );
		actor->setIFF( _iff );
		actor->setActiove( _active );

		m_actors.push_back( actor );

		return actor;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::removeActor( const CollisionActorPtr & _actor )
	{
		_actor->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::update()
	{
		for( TVectorCollisionActor::iterator
			it = m_actors.begin(),
			it_end = m_actors.end();
		it != it_end;
		++it )
		{
			const CollisionActorPtr & actor = *it;

			if( actor->isRemoved() == true )
			{
				continue;
			}

			if( actor->isActive() == false )
			{
				continue;
			}

			uint32_t actor_iff = actor->getIFF();

			mt::capsule2 actor_capsule;
			actor->makeCapsule( actor_capsule );

			CollisionActorProviderInterface * actor_provider = actor->getCollisionActorProvider();

			CollisionActorProviderInterface * actor_test_provider = nullptr;
			mt::vec2f collision_point;
			mt::vec2f collision_normal;
			float collision_penetration = 10000.f;

			for( TVectorCollisionActor::iterator
				it_test = m_actors.begin(),
				it_test_end = m_actors.end();
			it_test != it_test_end;
			++it_test )
			{
				const CollisionActorPtr & actor_test = *it_test;

				if( actor_test->isRemoved() == true )
				{
					continue;
				}

				if( actor == actor_test )
				{
					continue;
				}

				uint32_t actor_test_iff = actor_test->getIFF();

				if( m_iffs[actor_iff][actor_test_iff] == false )
				{
					continue;
				}

				mt::capsule2 actor_test_capsule;
				actor_test->makeCapsule( actor_test_capsule );

				mt::vec2f test_collision_point;
				mt::vec2f test_collision_normal;
				float test_collision_penetration;
				if( mt::capsule2_intersect_capsule2( actor_capsule, actor_test_capsule, test_collision_point, test_collision_normal, test_collision_penetration ) == false )
				{
					continue;
				}

				if( test_collision_penetration < collision_penetration )
				{
					collision_point = test_collision_point;
					collision_normal = test_collision_normal;
					collision_penetration = test_collision_penetration;

					actor_test_provider = actor_test->getCollisionActorProvider();
				}				
			}

			if( actor_test_provider != nullptr )
			{
				actor_provider->onCollisionTest( actor_test_provider, collision_point, collision_normal, collision_penetration );
			}
		}

		TVectorCollisionActor::iterator it_actors_erase = std::remove_if( m_actors.begin(), m_actors.end(), FActorDead() );
		m_actors.erase( it_actors_erase, m_actors.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::remove()
	{
		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionWorld::isRemoved() const
	{
		return m_remove;
	}
}