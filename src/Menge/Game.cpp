#	include "Game.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/WatchdogInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/TextInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"
#   include "Interface/PlayerInterface.h"

#   include "Kernel/Arrow.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Core/String.h"
#	include "Core/Stream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AccountService );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Menge::Game );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	GameAccountProvider::GameAccountProvider( ServiceProviderInterface * _serviceProvider, Game * _game )
		: m_serviceProvider(_serviceProvider)
		, m_game(_game)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onCreateAccount( const WString & _accountID )
	{
        EVENTABLE_METHOD( m_game, EVENT_GAME_CREATE_ACCOUNT )
            ->onGameCreateAccount( _accountID );
		//EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_CREATE_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onDeleteAccount( const WString & _accountID )
	{
        EVENTABLE_METHOD( m_game, EVENT_GAME_DELETE_ACCOUNT )
            ->onGameDeleteAccount( _accountID );
		//EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_DELETE_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onSelectAccount( const WString & _accountID )
	{
        EVENTABLE_METHOD( m_game, EVENT_GAME_SELECT_ACCOUNT )
            ->onGameSelectAccount( _accountID );
		//EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_SELECT_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onUnselectAccount( const WString & _accountID )
	{
        EVENTABLE_METHOD( m_game, EVENT_GAME_UNSELECT_ACCOUNT )
            ->onGameUselectAccount( _accountID );
		//EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_UNSELECT_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	GameSoundVolumeProvider::GameSoundVolumeProvider( ServiceProviderInterface * _serviceProvider, Game * _game )
		: m_serviceProvider(_serviceProvider)
		, m_game(_game)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSoundVolumeProvider::onSoundChangeVolume( float _sound, float _music, float _voice )
	{
        EVENTABLE_METHOD( m_game, EVENT_GAME_CHANGE_SOUND_VOLUME )
            ->onGameChangeSoundVolume( _sound, _music, _voice );
		//EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_CHANGE_SOUND_VOLUME)( _sound, _music, _voice );
	}
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_accountProvider(nullptr)
		, m_soundVolumeProvider(nullptr)
		, m_defaultArrow(nullptr)
		, m_timingFactor(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( const InputKeyEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_KEY, handle )
                ->onGameKey( _event.key, wp.x, wp.y, _event.code, _event.isDown, _event.isRepeat );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_KEY, handle )((uint32_t)_event.key, wp.x, wp.y, _event.code, _event.isDown, _event.isRepeat);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE(m_serviceProvider)
				->handleKeyEvent( _event );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_BUTTON, handle )
                ->onGameMouseButton( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_BUTTON, handle )(_event.touchId, wp.x, wp.y, _event.button, _event.isDown);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseButtonEvent( _event );
		}
                    
		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_BUTTON_BEGIN, handle )
                ->onGameMouseButtonBegin( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_BUTTON_BEGIN, handle )(_event.touchId, wp.x, wp.y, _event.button, _event.isDown);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseButtonEventBegin( _event );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_BUTTON_END, handle )
                ->onGameMouseButtonEnd( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_BUTTON_END, handle )(_event.touchId, wp.x, wp.y, _event.button, _event.isDown);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseButtonEventEnd( _event );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( const InputMouseMoveEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			mt::vec2f point( _event.x, _event.y );
			mt::vec2f delta( _event.dx, _event.dy );

			mt::vec2f wp;
			PLAYER_SERVICE(m_serviceProvider)
				->calcGlobalMouseWorldPosition( point, wp );

			mt::vec2f wd;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldDelta( point, delta, wd );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_MOVE, handle )
                ->onGameMouseMove( _event.touchId, wp.x, wp.y, wd.x, wd.y );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_MOVE, handle )(_event.touchId, wp.x, wp.y, wd.x, wd.y);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseMove( _event );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseWheel( const InputMouseWheelEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_WHEEL, handle )
                ->onGameMouseWheel( _event.button, _event.x, _event.y, _event.wheel );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_WHEEL, handle )(_event.button, _event.x, _event.y, _event.wheel);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseWheel( _event );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseLeave( const InputMousePositionEvent & _event )
	{		
        EVENTABLE_METHOD( this, EVENT_GAME_APP_MOUSE_LEAVE )
            ->onGameAppMouseLeave();
		//EVENTABLE_CALL(m_serviceProvider, this, EVENT_GAME_APP_MOUSE_LEAVE)();

		PLAYER_SERVICE( m_serviceProvider )
			->onAppMouseLeave( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseEnter( const InputMousePositionEvent & _event )
	{
        EVENTABLE_METHOD( this, EVENT_GAME_APP_MOUSE_ENTER )
            ->onGameAppMouseEnter( _event.x, _event.y );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_APP_MOUSE_ENTER )(_event.x, _event.y);

		PLAYER_SERVICE( m_serviceProvider )
			->onAppMouseEnter( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mousePosition( const InputMousePositionEvent & _event )
	{
		PLAYER_SERVICE( m_serviceProvider )
			->onAppMousePosition( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update()
	{
		m_events.insert( m_events.end(), m_eventsAdd.begin(), m_eventsAdd.end() );
		m_eventsAdd.clear();

		for( TVectorUserEvents::const_iterator
			it = m_events.begin(),
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			const UserEvent & ev = *it;

            EVENTABLE_METHOD( this, EVENT_GAME_USER )
                ->onGameUser( ev.id, ev.params );
		}

		m_events.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::tick( float _time, float _timing )
	{
		float timing = _timing * m_timingFactor;
		
		PLAYER_SERVICE( m_serviceProvider )
			->tick( _time, timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render()
	{
		PLAYER_SERVICE( m_serviceProvider )
			->render();
	}
    //////////////////////////////////////////////////////////////////////////
	void Game::registerEventMethods_( const ScriptModuleInterfacePtr & _module )
    {	
        _module->registerEventMethod( this, EVENT_GAME_FULLSCREEN, "onFullscreen" );
		_module->registerEventMethod( this, EVENT_GAME_FIXED_CONTENT_RESOLUTION, "onFixedContentResolution" );
		_module->registerEventMethod( this, EVENT_GAME_RENDER_VIEWPORT, "onRenderViewport" );
		_module->registerEventMethod( this, EVENT_GAME_VIEWPORT, "onGameViewport" );
		
		_module->registerEventMethod( this, EVENT_GAME_KEY, "onHandleKeyEvent" );
		_module->registerEventMethod( this, EVENT_GAME_MOUSE_BUTTON, "onHandleMouseButtonEvent" );
		_module->registerEventMethod( this, EVENT_GAME_MOUSE_BUTTON_BEGIN, "onHandleMouseButtonEventBegin" );
		_module->registerEventMethod( this, EVENT_GAME_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd" );
		_module->registerEventMethod( this, EVENT_GAME_MOUSE_MOVE, "onHandleMouseMove" );
		_module->registerEventMethod( this, EVENT_GAME_MOUSE_WHEEL, "onHandleMouseWheel" );

		_module->registerEventMethod( this, EVENT_GAME_APP_MOUSE_ENTER, "onAppMouseEnter" );
		_module->registerEventMethod( this, EVENT_GAME_APP_MOUSE_LEAVE, "onAppMouseLeave" );

		_module->registerEventMethod( this, EVENT_GAME_ON_TIMING_FACTOR, "onTimingFactor" );

		_module->registerEventMethod( this, EVENT_GAME_PREPARATION, "onPreparation" );
		_module->registerEventMethod( this, EVENT_GAME_RUN, "onRun" );
		_module->registerEventMethod( this, EVENT_GAME_INITIALIZE, "onInitialize" );
		_module->registerEventMethod( this, EVENT_GAME_INITIALIZE_RENDER_RESOURCES, "onInitializeRenderResources" );
		_module->registerEventMethod( this, EVENT_GAME_ACCOUNT_FINALIZE, "onAccountFinalize" );
		_module->registerEventMethod( this, EVENT_GAME_FINALIZE, "onFinalize" );
		_module->registerEventMethod( this, EVENT_GAME_DESTROY, "onDestroy" );

		_module->registerEventMethod( this, EVENT_GAME_FOCUS, "onFocus" );

		_module->registerEventMethod( this, EVENT_GAME_CREATE_DEFAULT_ACCOUNT, "onCreateDefaultAccount" );
		_module->registerEventMethod( this, EVENT_GAME_LOAD_ACCOUNTS, "onLoadAccounts" );

		_module->registerEventMethod( this, EVENT_GAME_CREATE_ACCOUNT, "onCreateAccount" );
		_module->registerEventMethod( this, EVENT_GAME_DELETE_ACCOUNT, "onDeleteAccount" );
		_module->registerEventMethod( this, EVENT_GAME_SELECT_ACCOUNT, "onSelectAccount" );
		_module->registerEventMethod( this, EVENT_GAME_UNSELECT_ACCOUNT, "onUnselectAccount" );

		_module->registerEventMethod( this, EVENT_GAME_CHANGE_SOUND_VOLUME, "onChangeSoundVolume" );

		_module->registerEventMethod( this, EVENT_GAME_CURSOR_MODE, "onCursorMode" );

		_module->registerEventMethod( this, EVENT_GAME_USER, "onUserEvent" );
		_module->registerEventMethod( this, EVENT_GAME_CLOSE, "onCloseWindow" );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality()
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );

        SCRIPT_SERVICE(m_serviceProvider)
			->addGlobalModule( "_DEVELOPMENT", (ScriptObject *)pybind::get_bool( developmentMode ) );

#   ifdef MENGINE_MASTER_RELEASE
        SCRIPT_SERVICE(m_serviceProvider)
            ->addGlobalModule( "_MASTER_RELEASE", (ScriptObject *)pybind::get_bool(true) );
#   else
        SCRIPT_SERVICE(m_serviceProvider)
            ->addGlobalModule( "_MASTER_RELEASE", (ScriptObject *)pybind::get_bool(false) );
#   endif

		if( SCRIPT_SERVICE(m_serviceProvider)
			->bootstrapModules() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Game::loadPersonality invalid bootstrap modules"
				);

			return false;
		}

		ConstString personality = CONFIG_VALUE( m_serviceProvider, "Game", "PersonalityModule", STRINGIZE_STRING_LOCAL( m_serviceProvider, "Personality" ) );

		ScriptModuleInterfacePtr module = SCRIPT_SERVICE( m_serviceProvider )
			->importModule( personality );

		if( module == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Game::loadPersonality invalid import module '%s'"
				, personality.c_str()
				);

			return false;
		}

		this->registerEventMethods_( module );

#	ifdef _DEBUG
		bool is_debug = true;
#	else
		bool is_debug = false;
#	endif
		
        LOGGER_WARNING(m_serviceProvider)("Preparation debug(%d)"
            , is_debug
            );

        bool result = EVENTABLE_METHODR( this, EVENT_GAME_PREPARATION, true )
            ->onGamePreparation( is_debug );
		//bool result = true;
		//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_PREPARATION, result )( is_debug );

		if( result == false )
		{
			return false;
		}

		if( SCRIPT_SERVICE( m_serviceProvider )
			->initializeModules() == false )
		{
			return false;
		}

		m_defaultArrow = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototypeT<Arrow *>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Arrow" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Default" ) );

		if( m_defaultArrow == nullptr )
		{
			LOGGER_WARNING( m_serviceProvider )("Game::initialize failed create defaultArrow 'Default'"
				);
		}
		else
		{
			m_defaultArrow->setName( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Default" ) );

			PLAYER_SERVICE( m_serviceProvider )
				->setArrow( m_defaultArrow );
		}

        bool EVENT_INITIALIZE_result = EVENTABLE_METHODR( this, EVENT_GAME_INITIALIZE, true )
            ->onGameInitialize();

		//bool EVENT_INITIALIZE_result = true;
		//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_INITIALIZE, EVENT_INITIALIZE_result )();

		if( EVENT_INITIALIZE_result == false )
		{
			return false;
		}

		if( ACCOUNT_SERVICE( m_serviceProvider )
			->loadAccounts() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Game::initialize failed load accounts"
				);
		}

		bool hasCurrentAccount = ACCOUNT_SERVICE( m_serviceProvider )
			->hasCurrentAccount();

		if( hasCurrentAccount == false )
		{
            EVENTABLE_METHOD( this, EVENT_GAME_CREATE_DEFAULT_ACCOUNT )
                ->onGameCreateDefaultAccount();
			//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_CREATE_DEFAULT_ACCOUNT )();
		}

        EVENTABLE_METHOD( this, EVENT_GAME_LOAD_ACCOUNTS )
            ->onGameLoadAccounts();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_LOAD_ACCOUNTS )();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::_initialize()
	{
		CONFIG_SECTION( m_serviceProvider, "Params", m_params );

		SERVICE_CREATE( m_serviceProvider, AccountService );

		m_accountProvider = new GameAccountProvider( m_serviceProvider, this );

		ACCOUNT_SERVICE( m_serviceProvider )
			->setAccountProviderInterface( m_accountProvider );

		m_soundVolumeProvider = new GameSoundVolumeProvider( m_serviceProvider, this );

		SOUND_SERVICE( m_serviceProvider )
			->addSoundVolumeProvider( m_soundVolumeProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::run()
	{
		LOGGER_WARNING(m_serviceProvider)("Run game"
			);

        EVENTABLE_METHOD( this, EVENT_GAME_RUN )
            ->onGameRun();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_RUN )();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::destroyArrow()
	{
		if( m_defaultArrow != nullptr )
		{
			m_defaultArrow->destroy();
			m_defaultArrow = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::_finalize()
	{	
        EVENTABLE_METHOD( this, EVENT_GAME_ACCOUNT_FINALIZE )
            ->onGameAccountFinalize();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_ACCOUNT_FINALIZE )();

		SERVICE_FINALIZE( m_serviceProvider, Menge::AccountServiceInterface );

		SCRIPT_SERVICE( m_serviceProvider )
			->finalizeModules();

        EVENTABLE_METHOD( this, EVENT_GAME_FINALIZE )
            ->onGameFinalize();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_FINALIZE )();

        if( m_accountProvider != nullptr )
        {
            delete m_accountProvider;
            m_accountProvider = nullptr;
        }

		if( m_soundVolumeProvider != nullptr )
		{
			delete m_soundVolumeProvider;
			m_soundVolumeProvider = nullptr;
		}

		this->destroyArrow();		      
				
        EVENTABLE_METHOD( this, EVENT_GAME_DESTROY )
            ->onGameDestroy();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_DESTROY )();

		this->removeEvents();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initializeRenderResources()
	{
        EVENTABLE_METHOD( this, EVENT_GAME_INITIALIZE_RENDER_RESOURCES )
            ->onGameInitializeRenderResources();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_INITIALIZE_RENDER_RESOURCES )();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalizeRenderResources()
	{
        EVENTABLE_METHOD( this, EVENT_GAME_FINALIZE_RENDER_RESOURCES )
            ->onGameFinalizeRenderResources();
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_FINALIZE_RENDER_RESOURCES )();
	}	
	//////////////////////////////////////////////////////////////////////////
	void Game::turnSound( bool _turn )
	{
        (void)_turn;
		
		if( _turn == true )
		{
			if( AMPLIFIER_SERVICE( m_serviceProvider ) )
			{
				AMPLIFIER_SERVICE( m_serviceProvider )->resume();
			}
		}
		else
		{
			if( AMPLIFIER_SERVICE( m_serviceProvider ) )
			{
				AMPLIFIER_SERVICE( m_serviceProvider )->pause();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFocus( bool _focus )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFocus( _focus );
		}				

        EVENTABLE_METHOD( this, EVENT_GAME_FOCUS )
            ->onGameFocus( _focus );
		//EVENTABLE_CALL(m_serviceProvider, this, EVENT_GAME_FOCUS)( _focus );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFullscreen( _resolution, _fullscreen );
		}

        EVENTABLE_METHOD( this, EVENT_GAME_FULLSCREEN )
            ->onGameFullscreen( _fullscreen );
		//EVENTABLE_CALL(m_serviceProvider, this, EVENT_GAME_FULLSCREEN)( _fullscreen );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFixedContentResolution( _resolution, _fixed );
		}

        EVENTABLE_METHOD( this, EVENT_GAME_FIXED_CONTENT_RESOLUTION )
            ->onGameFixedContentResolution( _fixed );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_FIXED_CONTENT_RESOLUTION )(_fixed);
    }
    //////////////////////////////////////////////////////////////////////////
    void Game::setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        EVENTABLE_METHOD( this, EVENT_GAME_RENDER_VIEWPORT )
            ->onGameRenderViewport( _viewport, _contentResolution );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_RENDER_VIEWPORT )(_viewport, _contentResolution);
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setGameViewport( const Viewport & _viewport, float _aspect )
	{
        EVENTABLE_METHOD( this, EVENT_GAME_VIEWPORT )
            ->onGameViewport( _viewport, _aspect );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_VIEWPORT )(_viewport, _aspect);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::close()
	{
		bool needQuit = EVENTABLE_METHODR( this, EVENT_GAME_VIEWPORT, true )
            ->onGameClose();

		//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_CLOSE, needQuit )();
	
		return needQuit;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::userEvent( const ConstString & _id, const TMapParams & _params )
    {
		UserEvent ev;
		ev.id = _id;
		ev.params = _params;

		m_events.push_back( ev );
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setCursorMode( bool _mode )
	{
        EVENTABLE_METHOD( this, EVENT_GAME_CURSOR_MODE )
            ->onGameCursorMode( _mode );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_CURSOR_MODE )(_mode);
	}
	//////////////////////////////////////////////////////////////////////////
	float Game::getTimingFactor() const
	{
		return m_timingFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setTimingFactor( float _timingFactor )
	{
		m_timingFactor = _timingFactor;

        EVENTABLE_METHOD( this, EVENT_GAME_ON_TIMING_FACTOR )
            ->onGameTimingFactor( _timingFactor );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_ON_TIMING_FACTOR )(m_timingFactor);
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Game::getParam( const ConstString & _paramName ) const
	{
		TMapParams::const_iterator it_find = m_params.find( _paramName );

        if( it_find == m_params.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("Game::getParam not found param '%s'"
                , _paramName.c_str()
                );

            return Utils::emptyWString();
        }

        const WString & param = it_find->second;

		return param;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::hasParam( const ConstString & _paramName ) const
	{
		TMapParams::const_iterator it_find = m_params.find( _paramName );

		if( it_find == m_params.end() )
		{
			return false;
		}

		return true;
	}
}
