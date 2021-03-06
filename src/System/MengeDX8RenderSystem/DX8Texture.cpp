/*
 *	DX8Texture.cpp
 *
 *	Created by _Berserk_ on 22.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX8Texture.h"

#	include <d3d8.h>
#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX8Texture::DX8Texture()
		: m_d3dInterface(nullptr)
        , m_hwWidth(0)
        , m_hwHeight(0)
        , m_hwChannels(0)
        , m_hwPixelFormat(PF_UNKNOWN)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX8Texture::~DX8Texture()
	{
        if( m_d3dInterface )
        {
            m_d3dInterface->Release();
            m_d3dInterface = nullptr;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void DX8Texture::initialize( IDirect3DTexture8 * _d3dInterface, size_t _hwWidth, size_t _hwHeight, size_t _hwChannels, PixelFormat _hwPixelFormat )
    {
        m_d3dInterface = _d3dInterface;
        m_hwWidth = _hwWidth;
        m_hwHeight = _hwHeight;
        m_hwChannels = _hwChannels;
        m_hwPixelFormat = _hwPixelFormat;
    }
	///////////////////////////////////////////////////////////////////////////
	unsigned char * DX8Texture::lock( int* _pitch, const Rect& _rect, bool _readOnly )
	{
		int flags;
		if( _readOnly )
		{
			flags=D3DLOCK_READONLY;
		}
		else 
		{
			flags=0;
		}

		RECT rect;
		rect.top = _rect.top;
		rect.bottom = _rect.bottom;
		rect.left = _rect.left;
		rect.right = _rect.right;

		D3DLOCKED_RECT TRect;
		HRESULT hr = m_d3dInterface->LockRect(0, &TRect, &rect, flags);
		if(FAILED( hr ))
		{
			return nullptr;
		}

		*_pitch = TRect.Pitch;
		return (unsigned char *)TRect.pBits;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8Texture::unlock()
	{
		m_d3dInterface->UnlockRect(0);
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DTexture8 * DX8Texture::getDXTextureInterface() const
	{
		return m_d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX8Texture::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX8Texture::getHWHeight() const
	{
		return m_hwHeight;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat DX8Texture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t DX8Texture::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
