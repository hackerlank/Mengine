#	pragma once

#	include "Kernel/Node.h"

#	include "AreaOfInterest.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    enum NodeAOITriggerEventFlag
    {
        EVENT_NODE_AOI_TRIGGER_ENTER = 0,
        EVENT_NODE_AOI_TRIGGER_LEAVE
    };
    //////////////////////////////////////////////////////////////////////////
    class NodeAOITriggerEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onNodeAOITriggerEnter( AOIActorProviderInterface * _enemy, uint32_t _iff1, uint32_t _iff2 ) = 0;
        virtual void onNodeAOITriggerLeave( AOIActorProviderInterface * _enemy, uint32_t _iff1, uint32_t _iff2 ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	class NodeAOITrigger
		: public Node
		, public Eventable
		, public AOITriggerProviderInterface
	{
        EVENT_RECEIVER( NodeAOITriggerEventReceiver );

	public:
		NodeAOITrigger();
		~NodeAOITrigger();

	public:
		void setRadius( float _radius );
		float getRadius() const;
				
	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;

	public:
		void setAOI( AreaOfInterest * _aoi );
		AreaOfInterest * getAOI() const;

	public:
		void setTriggerUserData( const pybind::object & _data );
		const pybind::object & getTriggerUserData() const;

	protected:
		bool onAOIActorTest( AOIActorProviderInterface * _actor ) const override;
		void onAOIActorEnter( AOIActorProviderInterface * _actor ) override;
		void onAOIActorLeave( AOIActorProviderInterface * _actor ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		AreaOfInterest * m_aoi;
		AOITriggerPtr m_trigger;

		pybind::object m_data;
		
		float m_radius;
		
		uint32_t m_iff;
	};
}