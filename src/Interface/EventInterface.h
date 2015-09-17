#   pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
    enum EEventName
    {
        __EVENT_BEGIN__ = 0,
        EVENT_DESTROY,
        EVENT_FULLSCREEN,
        EVENT_FIXED_CONTENT_RESOLUTION,
        EVENT_RENDER_VIEWPORT,
		EVENT_GAME_VIEWPORT,
        EVENT_ANIMATION_END,
        EVENT_FRAME_END,
        EVENT_FRAME_TICK,
        EVENT_PARTICLE_EMITTER_END,
        EVENT_PARTICLE_EMITTER_RESTART,
        EVENT_MOVE_END,
        EVENT_MOVE_STOP,
        EVENT_ROTATE_END,
        EVENT_ROTATE_STOP,
        EVENT_SCALE_END,
        EVENT_SCALE_STOP,
        EVENT_COLLIDE_BEGIN,
        EVENT_COLLIDE,
        EVENT_COLLIDE_END,
        EVENT_KEY,
		EVENT_KEY2,
        EVENT_MOUSE_BUTTON,
		EVENT_MOUSE_BUTTON2,
        EVENT_MOUSE_BUTTON_BEGIN,
        EVENT_MOUSE_BUTTON_END,
        EVENT_MOUSE_MOVE,
		EVENT_MOUSE_WHEEL,
        EVENT_GLOBAL_MOUSE_BUTTON,
		EVENT_GLOBAL_MOUSE_BUTTON2,
        EVENT_GLOBAL_MOUSE_BUTTON_BEGIN,
        EVENT_GLOBAL_MOUSE_BUTTON_END,
        EVENT_GLOBAL_MOUSE_MOVE,
		EVENT_GLOBAL_MOUSE_WHEEL,
        EVENT_MOUSE_ENTER,
        EVENT_MOUSE_LEAVE,
		EVENT_MOUSE_OVER_DESTROY,
        EVENT_APP_MOUSE_ENTER,
        EVENT_APP_MOUSE_LEAVE,
        EVENT_GLOBAL_KEY,
		EVENT_GLOBAL_KEY2,
        EVENT_YAWTIME_END,
        EVENT_PITCHTIME_END,
        EVENT_SOUND_END,
        EVENT_SOUND_PAUSE,
        EVENT_COLOR_END,
        EVENT_COLOR_STOP,
        EVENT_VIDEO_END,
        EVENT_FOCUS,
        EVENT_USER,
        EVENT_CLOSE,
        EVENT_PREPARATION,
        EVENT_ACTIVATE,
        EVENT_PREPARATION_DEACTIVATE,
        EVENT_DEACTIVATE,
        EVENT_COMPILE,
        EVENT_RELEASE,
		EVENT_RUN,
        EVENT_INITIALIZE,
        EVENT_INITIALIZE_RENDER_RESOURCES,
		EVENT_FINALIZE_RENDER_RESOURCES,
        EVENT_PARAMS,
        EVENT_ACCOUNT_FINALIZE,
        EVENT_FINALIZE,
        EVENT_CREATE,
		EVENT_LOAD_ACCOUNTS,
		EVENT_CREATE_DEFAULT_ACCOUNT,
        EVENT_CREATE_ACCOUNT,
        EVENT_DELETE_ACCOUNT,
        EVENT_SELECT_ACCOUNT,
        EVENT_UNSELECT_ACCOUNT,
		EVENT_CHANGE_SOUND_VOLUME,
        EVENT_ON_SUB_SCENE,
        EVENT_SCHEDULE,
        EVENT_TIMING,
        EVENT_AFFECTOR,
        EVENT_AFFECTOR_END,
        EVENT_PARTICLE,
		EVENT_SPINE_END,
		EVENT_SPINE_STOP,
		EVENT_SPINE_PAUSE,
		EVENT_SPINE_RESUME,
		EVENT_SPINE_EVENT,
        EVENT_MOVIE_GET_INTERNAL,
        EVENT_MOVIE_HIDE_INTERNAL,
        EVENT_MOVIE_ACTIVATE_INTERNAL,
        EVENT_MOVIE_DEACTIVATE_INTERNAL,
        EVENT_MOVIE_APPLY_INTERNAL_TRANSFORMATION,
        EVENT_MOVIE_APPLY_INTERNAL_OPACITY,
        EVENT_MOVIE_END,
		EVENT_MOVIE_LOOP,
        EVENT_ON_TIMING_FACTOR,
        EVENT_CURSOR_MODE,
		EVENT_TRIGGER_ENTER,
		EVENT_TRIGGER_LEAVE,
		EVENT_KEEP_SCRIPT,
		EVENT_RELEASE_SCRIPT,
        __EVENT_LAST__
    };
}