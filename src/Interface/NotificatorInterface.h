#   pragma once

namespace Menge
{
    enum ENotificatorName
    {
        __NOTIFICATOR_BEGIN__ = 0,
		NOTIFICATOR_DEBUG_OPEN_FILE,
		NOTIFICATOR_DEBUG_CLOSE_FILE,
        NOTIFICATOR_CHANGE_WINDOW_RESOLUTION,
        NOTIFICATOR_RESOURCE_COMPILE,
        NOTIFICATOR_RESOURCE_RELEASE,
		NOTIFICATOR_CHANGE_LOCALE,
		NOTIFICATOR_UPDATE_LOCALE_NODE,
		__NOTIFICATOR_END__,
    };
}
