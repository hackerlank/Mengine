#	include "DX9RenderImage.h"

#	include "DX9ErrorHelper.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9RenderImage::DX9RenderImage()
		: m_serviceProvider(nullptr)
		, m_d3dTexture(nullptr)
		, m_mode(ERIM_NORMAL)
        , m_hwWidth(0)
        , m_hwHeight(0)
        , m_hwChannels(0)
        , m_hwPixelFormat(PF_UNKNOWN)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderImage::~DX9RenderImage()
	{
        if( m_d3dTexture != nullptr )
        {			
            ULONG ref = m_d3dTexture->Release();
			(void)ref;

            m_d3dTexture = nullptr;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::initialize( ServiceProviderInterface * _serviceProvider, IDirect3DTexture9 * _d3dInterface, ERenderImageMode _mode, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, PixelFormat _hwPixelFormat )
    {
		m_serviceProvider = _serviceProvider;
        m_d3dTexture = _d3dInterface;

		m_mode = _mode;

        m_hwWidth = _hwWidth;
        m_hwHeight = _hwHeight;
        m_hwChannels = _hwChannels;
        m_hwPixelFormat = _hwPixelFormat;
    }
	///////////////////////////////////////////////////////////////////////////
	void * DX9RenderImage::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
	{
		DWORD flags;
		if( _readOnly == true )
		{
			flags = D3DLOCK_READONLY;
		}
		else 
		{
			flags = 0;
		}

		RECT rect;
		rect.top = _rect.top;
		rect.bottom = _rect.bottom;
		rect.left = _rect.left;
		rect.right = _rect.right;

		D3DLOCKED_RECT TRect;
		IF_DXCALL( m_serviceProvider, m_d3dTexture, LockRect, (_level, &TRect, &rect, flags) )
		{
			return nullptr;
		}

		*_pitch = (size_t)TRect.Pitch;

		void * bits = TRect.pBits;

		return bits;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderImage::unlock( uint32_t _level )
	{
		DXCALL( m_serviceProvider, m_d3dTexture, UnlockRect, (_level) );
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DTexture9 * DX9RenderImage::getDXTextureInterface() const
	{
		return m_d3dTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode DX9RenderImage::getMode() const
	{
		return m_mode;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t DX9RenderImage::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t DX9RenderImage::getHWHeight() const
	{
		return m_hwHeight;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat DX9RenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImage::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t DX9RenderImage::getHWDepth() const
	{
		return 1; //ToDo
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
