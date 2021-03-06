#	pragma once

#	include "WindowsLayer/WindowsIncluder.h"

#	define UWM_MOUSE_LEAVE (WM_USER+1)

#	define UTIMER_MOUSE_EVENT 1

namespace Menge
{
	class Win32MouseEvent
	{
	public:
		Win32MouseEvent();
		~Win32MouseEvent();

	public:
		bool initialize( HWND _hWnd );

	public:
		void verify();
		void update();

	public:
		void stop();

	protected:
		HWND m_hWnd;

		UINT m_uTimer;		
	};
}