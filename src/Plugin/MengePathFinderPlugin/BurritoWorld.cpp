#	include "BurritoWorld.h"

#	include "Interface/NodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Math/ccd.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		class FBurritoNodeRemove
		{
		public:
			FBurritoNodeRemove( Node * _node )
				: node(_node)
			{
			}

		public:
			bool operator()( BurritoNode * _node ) const
			{
				return _node->node == node;
			}

		protected:
			Node * node;

		private:
			FBurritoNodeRemove & operator = ( const FBurritoNodeRemove & _name );
		};
		//////////////////////////////////////////////////////////////////////////
		class FBurritoUnitRemove
		{
		public:
			FBurritoUnitRemove( Node * _node )
				: node( _node )
			{
			}

		public:
			bool operator()( BurritoUnit * _unit ) const
			{
				Node * unit_node = _unit->getNode();

				return unit_node == node;
			}

		protected:
			Node * node;

		private:
			FBurritoUnitRemove & operator = (const FBurritoUnitRemove & _name);
		};
		//////////////////////////////////////////////////////////////////////////
		class FBurritoUnitDead
		{
		public:
			bool operator()( BurritoUnit * _unit ) const
			{
				if( _unit->isDead() == false )
				{
					return false;
				}

				return true;
			}

		private:
			FBurritoUnitDead & operator = (const FBurritoUnitDead & _name);
		};
		//////////////////////////////////////////////////////////////////////////
		static void s_burritoUnitBounds( const BurritoUnitBound & _bound, BurritoUnit * _unit, const mt::vec3f & _translate )
		{
			const mt::vec3f & unit_pos = _unit->getPosition();

			if( _bound.less == true )
			{
				if( unit_pos.x < _bound.value )
				{
					return;
				}

				if( unit_pos.x > _bound.value && unit_pos.x + _translate.x > _bound.value )
				{
					return;
				}
			}
			else
			{
				if( unit_pos.x > _bound.value )
				{
					return;
				}

				if( unit_pos.x < _bound.value && unit_pos.x + _translate.x < _bound.value )
				{
					return;
				}
			}

			Node * unit_node = _unit->getNode();

			_bound.cb.call( unit_node );
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::BurritoWorld()
		: m_serviceProvider(nullptr)
		, m_bison(nullptr)
		, m_dead(false)
		, m_freeze(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::~BurritoWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{ 
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::initialize()
	{ 
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::finalize()
	{
		for( TVectorBurritoGround::iterator
			it = m_grounds.begin(),
			it_end = m_grounds.end();
		it != it_end;
		++it )
		{
			BurritoGround * ground = *it;

			delete ground;
		}

		m_grounds.clear();

		if( m_bison != nullptr )
		{
			m_bison->finalize();

			delete m_bison;
			m_bison = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::setDead()
	{
		m_dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::isDead() const
	{ 
		return m_dead;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::setFreeze( bool _value )
	{
		m_freeze = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::getFreeze() const
	{
		return m_freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison * BurritoWorld::createBison( Node * _node, RenderCameraOrthogonal * _camera, const Viewport & _renderport, const mt::vec3f & _position, float _radius )
	{
		m_bison = new BurritoBison;

		m_bison->initialize( _node, _camera, _renderport, _position, _radius );

		return m_bison;
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoGround * BurritoWorld::createGround( float _x, float _y, float _z, float _d, const pybind::object & _cb )
	{
		BurritoGround * ground = new BurritoGround();

		mt::planef p( _x, _y, _z, _d );
		
		ground->initialize( p, _cb );

		m_grounds.push_back( ground );

		return ground;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::addUnitBounds( float _value, bool _less, const pybind::object & _cb )
	{
		BurritoUnitBound bound;
		bound.value = _value;
		bound.less = _less;
		bound.cb = _cb;

		m_unitBounds.push_back( bound );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createLayer( const ConstString & _layerName, const mt::vec3f & _parallax, uint32_t _count, float _width, const pybind::object & _cb )
	{
		BurritoLayer layer;
		layer.name = _layerName;
		layer.parallax = _parallax;

		layer.endless = new Endless;

		layer.endless->initialize( _count, _width, _parallax, _cb );

		m_layers.push_back( layer );
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoUnit * BurritoWorld::addLayerUnit( const ConstString & _layerName, Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, const pybind::object & _cb )
	{
		if( _node == nullptr )
		{
			return nullptr;
		}

		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _layerName )
			{
				continue;
			}

			BurritoUnit * unit = new BurritoUnit;
			unit->initialize( _node, _position, _velocity, _radius, _collide, _cb );
			
			layer.unitsAdd.push_back( unit );

			return unit;
		}

		LOGGER_ERROR( m_serviceProvider )("BurritoWorld::addLayerUnit not found layer %s"
			, _layerName.c_str()
			);

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::removeLayerUnit( const ConstString & _layerName, Node * _node )
	{
		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _layerName )
			{
				continue;
			}

			TVectorBurritoUnit::iterator it_add_erase = std::find_if( layer.unitsAdd.begin(), layer.unitsAdd.end(), FBurritoUnitRemove( _node ) );

			if( it_add_erase != layer.unitsAdd.end() )
			{
				(*it_add_erase)->setDead();

				layer.unitsAdd.erase( it_add_erase );

				break;
			}

			TVectorBurritoUnit::iterator it_erase = std::find_if( layer.units.begin(), layer.units.end(), FBurritoUnitRemove( _node ) );
			
			(*it_erase)->setDead();
			
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::addLayerBounds( const ConstString & _layerName, float _value, bool _less, const pybind::object & _cb )
	{ 
		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _layerName )
			{
				continue;
			}

			BurritoUnitBound bound;
			bound.value = _value;
			bound.less = _less;
			bound.cb = _cb;

			layer.unitBounds.push_back( bound );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::update( float _time, float _timing )
	{
		if( m_bison == nullptr )
		{
			return;
		}

		if( m_freeze == true )
		{
			return;
		}
		
		uint32_t iterate = 1;
		uint32_t total_iterate = 0;
		float next_timing = _timing;
		
		while( iterate-- != 0 )
		{
			float iterate_timing = next_timing;

			float bison_radius = m_bison->getRadius();

			mt::vec3f velocity;
			mt::vec3f position;
			m_bison->update( _time, iterate_timing, velocity, position, total_iterate );

			float translate_position = 0.f;

			float sq_speed = velocity.sqrlength();

			if( mt::equal_f_z( sq_speed ) == false )
			{
				if( m_bison->getCollide() == true )
				{
					bool collision = false;
					float collisionTiming = 0.f;

					for( TVectorBurritoLayer::const_iterator
						it = m_layers.begin(),
						it_end = m_layers.end();
					it != it_end;
					++it )
					{
						const BurritoLayer & layer = *it;

						for( TVectorBurritoUnit::const_iterator
							it_unit = layer.units.begin(),
							it_unit_end = layer.units.end();
						it_unit != it_unit_end;
						++it_unit )
						{
							const BurritoUnit * unit = *it_unit;

							mt::vec3f collision_velocity = velocity * layer.parallax;

							if( unit->check_collision( iterate_timing, position, bison_radius, collision_velocity, collisionTiming ) == true )
							{
								collision = true;

								break;
							}
						}
					}

					if( collision == false )
					{
						for( TVectorBurritoGround::iterator
							it = m_grounds.begin(),
							it_end = m_grounds.end();
						it != it_end;
						++it )
						{
							BurritoGround * ground = *it;

							if( ground->check_collision( iterate_timing, position, bison_radius, velocity, collisionTiming ) == true )
							{
								collision = true;
							}
						}
					}

					if( collision == true )
					{
						next_timing = iterate_timing - collisionTiming;
						iterate_timing = collisionTiming;

						++iterate;
						++total_iterate;
					}
				}
								
				mt::vec3f bison_translate = velocity * iterate_timing;
				
				m_bison->translate( bison_translate );

				translate_position = -bison_translate.x;				
				
				for( TVectorBurritoLayer::iterator
					it = m_layers.begin(),
					it_end = m_layers.end();
				it != it_end;
				++it )
				{
					BurritoLayer & layer = *it;

					float layer_translate_position = translate_position * layer.parallax.x;

					layer.endless->slide( layer_translate_position );
				}
			}

			for( TVectorBurritoLayer::iterator
				it = m_layers.begin(),
				it_end = m_layers.end();
			it != it_end;
			++it )
			{
				BurritoLayer & layer = *it;

				layer.units.insert( layer.units.end(), layer.unitsAdd.begin(), layer.unitsAdd.end() );
				layer.unitsAdd.clear();
			}

			for( TVectorBurritoLayer::iterator
				it = m_layers.begin(),
				it_end = m_layers.end();
			it != it_end;
			++it )
			{
				BurritoLayer & layer = *it;

				float layer_translate_position = translate_position * layer.parallax.x;

				for( TVectorBurritoUnit::iterator
					it_unit = layer.units.begin(),
					it_unit_end = layer.units.end();
				it_unit != it_unit_end;
				++it_unit )
				{
					BurritoUnit * unit = *it_unit;

					if( unit->isDead() == true )
					{
						continue;
					}

					const mt::vec3f & unit_velocity = unit->getVelocity();

					mt::vec3f unit_translate = unit_velocity * iterate_timing + mt::vec3f( layer_translate_position, 0.f, 0.f );
					
					for( TVectorBurritoUnitBounds::const_iterator
						it_bound = layer.unitBounds.begin(),
						it_bound_end = layer.unitBounds.end();
					it_bound != it_bound_end;
					++it_bound )
					{
						if( unit->isBound() == false )
						{
							break;
						}

						const BurritoUnitBound & bound = *it_bound;

						s_burritoUnitBounds( bound, unit, unit_translate );
					}

					for( TVectorBurritoUnitBounds::const_iterator
						it_bound = m_unitBounds.begin(),
						it_bound_end = m_unitBounds.end();
					it_bound != it_bound_end;
					++it_bound )
					{
						if( unit->isBound() == false )
						{
							break;
						}

						const BurritoUnitBound & bound = *it_bound;

						s_burritoUnitBounds( bound, unit, unit_translate );
					}
				}
			}

			for( TVectorBurritoLayer::iterator
				it = m_layers.begin(),
				it_end = m_layers.end();
			it != it_end;
			++it )
			{
				BurritoLayer & layer = *it;

				float layer_translate_position = translate_position * layer.parallax.x;

				for( TVectorBurritoUnit::iterator
					it_unit = layer.units.begin(),
					it_unit_end = layer.units.end();
				it_unit != it_unit_end;
				++it_unit )
				{
					BurritoUnit * unit = *it_unit;

					if( unit->isDead() == true )
					{
						continue;
					}

					const mt::vec3f & unit_velocity = unit->getVelocity();

					mt::vec3f unit_translate = unit_velocity * iterate_timing + mt::vec3f( layer_translate_position, 0.f, 0.f );

					unit->translate( unit_translate );
				}
			}

			for( TVectorBurritoLayer::iterator
				it = m_layers.begin(),
				it_end = m_layers.end();
			it != it_end;
			++it )
			{
				BurritoLayer & layer = *it;

				TVectorBurritoUnit::iterator it_erase = std::remove_if( layer.units.begin(), layer.units.end(), FBurritoUnitDead() );
				layer.units.erase( it_erase, layer.units.end() );
			}
		}
	}
}