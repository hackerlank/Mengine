#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "DX8RenderTexture.h"

#   include "Utils/Factory/FactoryPool.h"
#   include "Utils/Core/BinaryVector.h"

#	include <d3d8.h>

namespace Menge
{
	struct VBInfo
	{
		size_t length;
		size_t vertexSize;
		DWORD usage;
		DWORD fvf;
		D3DPOOL pool;
		IDirect3DVertexBuffer8* pVB;
	};

	struct IBInfo
	{
		size_t length;
		DWORD usage;
		D3DFORMAT format;
		D3DPOOL pool;
		IDirect3DIndexBuffer8* pIB;
	};

	class DX8RenderSystem
		: public RenderSystemInterface
	{
	public:
		DX8RenderSystem();
		~DX8RenderSystem();
        
    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    protected:
		void clear_( DWORD _color );
		bool supportNPOT_() const;
		bool onRestoreDevice_();

	public:
		bool initialize() override;
        void finalize() override;

    public:
        void setRenderListener( RenderSystemListener * _listener ) override;
		
	public:
		bool createRenderWindow( size_t _width, size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;
		
        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;

		void clear( uint32 _color ) override;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		void screenshot( const RenderImageInterfacePtr & _image, const float * _rect ) override;
		// ������� ������: ������� 4 �� 4
		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _modelview ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;
		void setWorldMatrix( const mt::mat4f & _world ) override;

		VBHandle createVertexBuffer( size_t _verticesNum, size_t _vertexSize ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( size_t _indiciesNum ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		uint16* lockIndexBuffer(  IBHandle _ibHandle ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) override;

		void setVertexDeclaration( size_t _vertexSize, uint32 _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type
			, size_t _baseVertexIndex
			, size_t _minIndex
			, size_t _verticesNum
			, size_t _startIndex
			, size_t _indexCount ) override;

		void setTexture( size_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
		void setTextureFactor( uint32 _color ) override;

		void setSrcBlendFactor( EBlendFactor _src ) override;
		void setDstBlendFactor( EBlendFactor _dst ) override;

		void setCullMode( ECullMode _mode ) override;

		void setDepthBufferTestEnable( bool _depthTest ) override;
		void setDepthBufferWriteEnable( bool _depthWrite ) override;
		void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;

		void setFillMode( EFillMode _mode ) override;
		void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
		void setShadeType( EShadeType _sType ) override;
		void setAlphaTestEnable( bool _alphaTest ) override;
		void setAlphaBlendEnable( bool _alphaBlend ) override;
		void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha ) override;
		void setLightingEnable( bool _light ) override;

		void setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;

        void setTextureStageTexCoordIndex( size_t _stage, size_t _index ) override;

		void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;
		
		// create empty render image
		RenderImageInterfacePtr createImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		RenderImageInterfacePtr createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		
		
		// create render target image
		RenderImageInterfacePtr createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		// ��������� �����������

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color = 0, float _depth = 1.0f, uint16 _stencil = 0 ) override;

		void setViewport( const Viewport & _viewport ) override;

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;
		bool setRenderTarget( const RenderImageInterfacePtr & _renderTarget, bool _clear ) override;

		bool supportTextureFormat( PixelFormat _format ) const override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;
		void setSeparateAlphaBlendMode() override;

    protected:
        bool resetDevice_();

    protected:
        PixelFormat findFormatFromChannels_( size_t & _channels, PixelFormat _format ) const;
        void fixNPOTSupport_( size_t & _width, size_t & _height ) const;

	private:
		ServiceProviderInterface * m_serviceProvider;

        RenderSystemListener* m_listener;

		HMODULE m_hd3d8;

		Resolution m_windowResolution;
		int m_screenBits;
		bool m_fullscreen;

		IDirect3D8*			m_pD3D;
		IDirect3DDevice8*	m_pD3DDevice;
		IDirect3DSurface8*	m_screenSurf;
		IDirect3DSurface8*	m_screenDepth;

		D3DPRESENT_PARAMETERS m_d3dppW;
		D3DPRESENT_PARAMETERS m_d3dppFS;

		D3DPRESENT_PARAMETERS * m_d3dpp;

		D3DMATRIX m_matTexture;

		// sync routines
		IDirect3DSurface8*	m_syncTargets[2];
		IDirect3DTexture8*	m_syncTempTex;
		IDirect3DSurface8*	m_syncTemp;
		unsigned int		m_frames;
		void syncCPU_();

		bool initLost_();
		
		void release_();
		bool restore_();

		void createSyncTargets_();

		HRESULT d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,
			DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8 * ppTexture );

		HRESULT loadSurfaceFromSurface_( LPDIRECT3DSURFACE8 pDestSurface, CONST RECT * pDestRect,
			LPDIRECT3DSURFACE8 pSrcSurface, CONST RECT * pSrcRect );

		bool m_inRender;

		DX8RenderTexturePtr m_curRenderTexture;

        mt::mat4f m_projection;
        mt::mat4f m_modelview;

        Viewport m_viewport;

		VBHandle m_vbHandleCounter;
		IBHandle m_ibHandleCounter;

		typedef BinaryVector<VBHandle, VBInfo> TMapVBInfo;
		TMapVBInfo m_vertexBuffers;

        IDirect3DVertexBuffer8 * m_currentVB;

		typedef BinaryVector<VBHandle, IBInfo> TMapIBInfo;
		TMapIBInfo m_indexBuffers;

        IDirect3DIndexBuffer8 * m_currentIB;

        typedef FactoryPool<DX8Texture, 128> TFactoryDX8Texture;
        TFactoryDX8Texture m_factoryDX8Texture;

        typedef FactoryPool<DX8RenderTexture, 4> TFactoryDX8RenderTexture;
        TFactoryDX8RenderTexture m_factoryDX8RenderTexture;
		
		bool m_syncReady;

        bool m_supportNPOT;
        bool m_supportL8;
        bool m_supportA8;
        bool m_supportR8G8B8;
	};
}	// namespace Menge
