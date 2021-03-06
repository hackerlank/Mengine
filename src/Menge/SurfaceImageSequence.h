#	pragma once

#	include "Kernel/Surface.h"
#	include "Kernel/Eventable.h"
#	include "Kernel/Animatable.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class ResourceAnimation> ResourceAnimationPtr;
    //////////////////////////////////////////////////////////////////////////
    enum SurfaceImageSequenceEventFlag
    {
        EVENT_SURFACE_IMAGESEQUENCE_FRAME_END = __EVENT_ANIMATABLE_LAST__,
        EVENT_SURFACE_IMAGESEQUENCE_FRAME_TICK,
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceImageSequenceEventReceiver
        : public AnimatableEventReceiver
    {
    public:
        virtual void onSurfaceImageSequenceFrameEnd( uint32_t _currentFrame ) = 0;
        virtual void onSurfaceImageSequenceFrameTick( uint32_t _currentFrame, uint32_t _frameCount ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	class SurfaceImageSequence
		: public Surface        
        , public Eventable
        , public Animatable		
	{
        EVENT_RECEIVER( SurfaceImageSequenceEventReceiver );

	public:
		SurfaceImageSequence();
		~SurfaceImageSequence();

	public:
		void setResourceAnimation( const ResourceAnimationPtr & _resourceAnimation );
		const ResourceAnimationPtr & getResourceAnimation() const;

	public:
		uint32_t getFrameCount() const;
		
		float getFrameDelay( uint32_t  _frame ) const;		

		void setCurrentFrame( uint32_t _frame );
		uint32_t getCurrentFrame() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        const ColourValue & getColour() const override;

	protected:
		void _setTiming( float _timming ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

	protected:
		bool _play( float _time ) override;
		bool _restart( uint32_t _enumerator, float _time ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator, float _time ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		bool update( float _current, float _timing ) override;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
		uint32_t getFrame_( float _timing, float & _delthaTiming ) const;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const;

	protected:
		ResourceHolder<ResourceAnimation> m_resourceAnimation;

		float m_frameTiming;

		uint32_t m_currentFrame;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SurfaceImageSequence> SurfaceImageSequencePtr;
}
