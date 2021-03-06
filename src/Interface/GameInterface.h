#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/PlayerInterface.h"
#   include "Interface/MemoryInterface.h"

#	include "Core/Params.h"

#	include "Core/Magic.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class GameServiceInterface
        : public ServiceInterface
        , public InputSystemHandler
    {
        SERVICE_DECLARE("GameService")

	public:
		virtual void run() = 0;

    public:
		virtual void update() = 0;
		virtual void tick( float _time, float _timing ) = 0;

	public:
        virtual void render() = 0;

    public:
        virtual void setCursorMode( bool _mode ) = 0;

    public:
        virtual float getTimingFactor() const = 0;
        virtual void setTimingFactor( float _timingFactor ) = 0;

    public:
        virtual const WString & getParam( const ConstString & _paramName ) const = 0;
        virtual bool hasParam( const ConstString & _paramName ) const = 0;

    public:
        virtual bool loadPersonality() = 0;

    public:
        virtual void initializeRenderResources() = 0;
        virtual void finalizeRenderResources() = 0;

	public:
		virtual void mousePosition( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseLeave( const InputMousePositionEvent & _event ) = 0;
		
	public:
        virtual void setFocus( bool _focus ) = 0;
        virtual void setFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void setFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
		virtual void setGameViewport( const Viewport & _viewport, float _aspect ) = 0;

        virtual bool close() = 0;

        virtual void userEvent( const ConstString & _event, const TMapParams & _params ) = 0;

        virtual void turnSound( bool _turn ) = 0;
    };

#   define GAME_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::GameServiceInterface)
}