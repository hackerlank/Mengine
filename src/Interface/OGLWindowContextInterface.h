/*
 *	OGLWindowContext.h
 *
 *	Created by _Berserk_ on 8.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	class OGLWindowContext
	{
	public:
		virtual bool initialize( uint32_t _width, uint32_t _height, uint32_t _bits, bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync ) = 0;
		virtual void swapBuffers() = 0;
		virtual void setVSync( bool _vsync ) = 0;
		virtual void setFullscreenMode( uint32_t _width, uint32_t _height, bool _fullscreen ) = 0;
	};

	void createWindowContext( OGLWindowContext** _pWindowContext );
	void releaseWindowContext( OGLWindowContext* _pWindowContext );
}	// namespace Menge
