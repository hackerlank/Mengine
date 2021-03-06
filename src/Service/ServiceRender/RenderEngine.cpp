#	include "RenderEngine.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/ConfigInterface.h"

//#	include "Megatextures.h"

#	include "Core/RenderUtil.h"

#	include "Logger/Logger.h"

#	include "stdex/memorycopy.h"
#	include "stdex/intrusive_ptr_base.h"

#	include <math.h>
#   include <memory.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Menge::RenderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine()
		: m_windowCreated( false )
		, m_vsync( false )
		, m_currentTextureStages( 0 )
		, m_currentRenderCamera( nullptr )
		, m_currentRenderViewport( nullptr )
		, m_currentRenderClipplane( nullptr )
		, m_currentRenderTarget( nullptr )
		, m_maxVertexCount( 0 )
		, m_maxIndexCount( 0 )
		, m_depthBufferWriteEnable( false )
		, m_alphaBlendEnable( false )
		, m_debugStepRenderMode( false )
		, m_debugRedAlertMode( false )
		, m_currentStage( nullptr )
		, m_nullTexture( nullptr )
		, m_renderVertexCount( 0 )
		, m_renderIndicesCount( 0 )
		, m_batchMode( ERBM_SMART )
		, m_currentMaterialId( 0 )
		, m_iterateRenderObjects( 0 )
		, m_limitRenderObjects( 0 )
		, m_stopRenderObjects( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::_initialize()
	{
		m_maxVertexCount = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderMaxVertexCount", 32000U );
		m_maxIndexCount = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderMaxIndexCount", 48000U );

		uint32_t maxObjects = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderMaxObject", 8000U );
		uint32_t maxPasses = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderMaxPass", 200U );

		uint32_t maxQuadBatch = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderMaxQuadBatch", 2000U );
		uint32_t maxLineBatch = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderMaxLineBatch", 4000U );

		m_renderObjects.reserve( maxObjects );
		m_renderPasses.reserve( maxPasses );

		m_indicesQuad.resize( maxQuadBatch * 6 );
		m_indicesLine.resize( maxLineBatch * 1 );

		for( uint32_t i = 0; i != maxQuadBatch; ++i )
		{   
			uint32_t indexOffset = i * 6;

			RenderIndices vertexOffset = (RenderIndices)i * 4;

			m_indicesQuad[indexOffset + 0] = vertexOffset + 0;
			m_indicesQuad[indexOffset + 1] = vertexOffset + 3;
			m_indicesQuad[indexOffset + 2] = vertexOffset + 1;
			m_indicesQuad[indexOffset + 3] = vertexOffset + 1;
			m_indicesQuad[indexOffset + 4] = vertexOffset + 3;
			m_indicesQuad[indexOffset + 5] = vertexOffset + 2;
		}

		for( uint32_t i = 0; i != maxLineBatch; ++i )
		{
			RenderIndices vertexOffset = (RenderIndices)i;

			m_indicesLine[i] = vertexOffset;
		}

		m_debugInfo.dips = 0;
		m_debugInfo.frameCount = 0;
		m_debugInfo.fillrate = 0.f;
		m_debugInfo.object = 0;
		m_debugInfo.triangle = 0;

		//m_megatextures = new Megatextures(2048.f, 2048.f, PF_A8R8G8B8);

		uint32_t batchMode = CONFIG_VALUE(m_serviceProvider, "Engine", "RenderServiceBatchMode", 2 );

		switch( batchMode )
		{
		case 0U:
			{
				m_batchMode = ERBM_NONE;
			}break;
		case 1U:
			{
				m_batchMode = ERBM_NORMAL;
			}break;
		case 2U:
			{
				m_batchMode = ERBM_SMART;
			}break;
		default:
			{
				m_batchMode = ERBM_SMART;
			}break;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::_finalize()
	{   
		for( TArrayRenderObject::iterator
			it = m_renderObjects.begin(),
			it_end = m_renderObjects.end();
		it != it_end;
		++it )
		{
			RenderObject & ro = *it;

			ro.material = nullptr;
		}

		m_renderObjects.clear();
		m_renderPasses.clear();
		//m_textures.clear();

		m_nullTexture = nullptr;
		m_whitePixelTexture = nullptr;
		
		m_vbHandle2D = nullptr;
		m_ibHandle2D = nullptr;

		m_currentVBHandle = nullptr;
		m_currentIBHandle = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen,
		int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_renderViewport = _renderViewport;

		m_fullscreen = _fullscreen;

		LOGGER_INFO(m_serviceProvider)("RenderEngine::createRenderWindow:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
			, m_windowResolution.getWidth()
			, m_windowResolution.getHeight()
			, m_contentResolution.getWidth()
			, m_contentResolution.getHeight()
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.end.x
			, m_renderViewport.end.y
			, m_fullscreen
			);
		
		m_windowCreated = RENDER_SYSTEM(m_serviceProvider)
			->createRenderWindow( m_windowResolution, _bits, m_fullscreen, m_vsync, _FSAAType, _FSAAQuality, _MultiSampleCount );

		if( m_windowCreated == false )
		{
			return false;
		}

		if( this->create2DBuffers_() == false )
		{
			return false;
		}

		this->restoreRenderSystemStates_();

		if( this->createNullTexture_() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::createRenderWindow invalid create __null__ texture"
				);

			return false;
		}

		if( this->createWhitePixelTexture_() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::createRenderWindow invalid create WhitePixel texture"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createNullTexture_()
	{
		uint32_t null_mipmaps = 1;
		uint32_t null_width = 2;
		uint32_t null_height = 2;
		uint32_t null_channels = 3;
		uint32_t null_depth = 1;

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createTexture( null_mipmaps, null_width, null_height, null_channels, null_depth, PF_UNKNOWN );

		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createNullTexture_ invalid create null texture %d:%d"
				, null_width
				, null_height
				);

			return false;
		}

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = null_width;
		rect.bottom = null_height;

		size_t pitch = 0;
		void * textureData = texture->lock( &pitch, 0, rect, false );

		if( textureData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createNullTexture_ invalid lock null texture %d:%d"
				, null_width
				, null_height
				);

			return false;
		}

		unsigned char * buffer_textureData = static_cast<unsigned char *>(textureData);

		buffer_textureData[0] = 0xFF;
		buffer_textureData[1] = 0x00;
		buffer_textureData[2] = 0x00;

		buffer_textureData[3] = 0x00;
		buffer_textureData[4] = 0xFF;
		buffer_textureData[5] = 0x00;

		buffer_textureData += pitch;

		buffer_textureData[0] = 0x00;
		buffer_textureData[1] = 0x00;
		buffer_textureData[2] = 0xFF;

		buffer_textureData[3] = 0x00;
		buffer_textureData[4] = 0x00;
		buffer_textureData[5] = 0x00;

		texture->unlock( 0 );

		RENDERTEXTURE_SERVICE(m_serviceProvider)
			->cacheFileTexture( ConstString::none(), STRINGIZE_STRING_LOCAL(m_serviceProvider, "__null__"), texture );

		m_nullTexture = texture;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createWhitePixelTexture_()
	{
		uint32_t null_mipmaps = 1;
		uint32_t null_width = 2;
		uint32_t null_height = 2;
		uint32_t null_channels = 3;
		uint32_t null_depth = 1;

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createTexture( null_mipmaps, null_width, null_height, null_channels, null_depth, PF_UNKNOWN );

		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createWhitePixelTexture_ invalid create null texture %d:%d"
				, null_width
				, null_height
				);

			return false;
		}

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = null_width;
		rect.bottom = null_height;

		size_t pitch = 0;
		void * textureData = texture->lock( &pitch, 0, rect, false );

		if( textureData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createWhitePixelTexture_ invalid lock null texture %d:%d"
				, null_width
				, null_height
				);

			return false;
		}

		unsigned char * buffer_textureData = static_cast<unsigned char *>(textureData);

		buffer_textureData[0] = 0xFF;
		buffer_textureData[1] = 0xFF;
		buffer_textureData[2] = 0xFF;

		buffer_textureData[3] = 0xFF;
		buffer_textureData[4] = 0xFF;
		buffer_textureData[5] = 0xFF;

		buffer_textureData += pitch;

		buffer_textureData[0] = 0xFF;
		buffer_textureData[1] = 0xFF;
		buffer_textureData[2] = 0xFF;

		buffer_textureData[3] = 0xFF;
		buffer_textureData[4] = 0xFF;
		buffer_textureData[5] = 0xFF;

		texture->unlock( 0 );

		RENDERTEXTURE_SERVICE(m_serviceProvider)
			->cacheFileTexture( ConstString::none(), STRINGIZE_STRING_LOCAL(m_serviceProvider, "WhitePixel"), texture );

		m_whitePixelTexture = texture;

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_renderViewport = _renderViewport;

		m_fullscreen = _fullscreen;

		LOGGER_INFO(m_serviceProvider)("RenderEngine::changeWindowMode:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
			, m_windowResolution.getWidth()
			, m_windowResolution.getHeight()
			, m_contentResolution.getWidth()
			, m_contentResolution.getHeight()
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.end.x
			, m_renderViewport.end.y
			, m_fullscreen
			);

		if( m_windowCreated == false )
		{
			return;
		}

		RENDER_SYSTEM(m_serviceProvider)
			->changeWindowMode( m_windowResolution, m_fullscreen );

		//this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( const RenderTextureInterfacePtr & _texture, const mt::vec4f & _rect )
	{
		const RenderImageInterfacePtr & image = _texture->getImage();

		RENDER_SYSTEM(m_serviceProvider)
			->screenshot( image, _rect );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowClose()
	{
		if( m_windowCreated == true )
		{
			RENDER_SYSTEM(m_serviceProvider)
				->onWindowClose();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginScene()
	{		
		m_renderPasses.clear();
		m_renderObjects.clear();

		m_debugVertices.clear();

		m_debugInfo.fillrate = 0.f;
		m_debugInfo.object = 0;
		m_debugInfo.triangle = 0;

		m_iterateRenderObjects = 0;
		m_stopRenderObjects = false;

		m_currentRenderCamera = nullptr;
		m_currentRenderViewport = nullptr;
		m_currentRenderClipplane = nullptr;
		m_currentRenderTarget = nullptr;
		
		m_currentMaterialId = 0;
		m_currentTextureStages = 0;
		m_currentStage = nullptr;

		m_renderVertexCount = 0;
		m_renderIndicesCount = 0;

		this->restoreRenderSystemStates_();
		
		if( RENDER_SYSTEM(m_serviceProvider)->beginScene() == false )
		{
			return false;
		}

		RENDER_SYSTEM( m_serviceProvider )
			->clear( 0x00000000, true );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		this->flushRender_();

		RENDER_SYSTEM(m_serviceProvider)
			->endScene();

		m_debugInfo.frameCount += 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		RENDER_SYSTEM(m_serviceProvider)
			->swapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isWindowCreated() const
	{
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateStage_( const RenderMaterialStage * _stage )
	{
		if( m_currentStage == _stage )
		{
			return;
		}
		
		m_currentStage = _stage;
		
		if( m_currentProgram != m_currentStage->program )
		{
			m_currentProgram = m_currentStage->program;
			
			RENDER_SYSTEM( m_serviceProvider )
				->setProgram( m_currentProgram );
		}

		for( uint32_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			RenderTextureStage & current_texture_stage = m_currentTextureStage[stageId];
			const RenderTextureStage & texture_stage = m_currentStage->textureStage[stageId];

			if( current_texture_stage.addressU != texture_stage.addressU
				|| current_texture_stage.addressV != texture_stage.addressV )
			{
				current_texture_stage.addressU = texture_stage.addressU;
				current_texture_stage.addressV = texture_stage.addressV;

				RENDER_SYSTEM( m_serviceProvider )->setTextureAddressing( stageId
					, current_texture_stage.addressU
					, current_texture_stage.addressV
					);
			}

			if( current_texture_stage.mipmap != texture_stage.mipmap ||
				current_texture_stage.magnification != texture_stage.magnification ||
				current_texture_stage.minification != texture_stage.minification )
			{
				current_texture_stage.mipmap = texture_stage.mipmap;
				current_texture_stage.magnification = texture_stage.magnification;
				current_texture_stage.minification = texture_stage.minification;

				RENDER_SYSTEM( m_serviceProvider )
					->setTextureStageFilter( stageId, current_texture_stage.minification, current_texture_stage.mipmap, current_texture_stage.magnification );
			}

			if( current_texture_stage.colorOp != texture_stage.colorOp
				|| current_texture_stage.colorArg1 != texture_stage.colorArg1
				|| current_texture_stage.colorArg2 != texture_stage.colorArg2 )
			{
				current_texture_stage.colorOp = texture_stage.colorOp;
				current_texture_stage.colorArg1 = texture_stage.colorArg1;
				current_texture_stage.colorArg2 = texture_stage.colorArg2;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( stageId
					, current_texture_stage.colorOp
					, current_texture_stage.colorArg1
					, current_texture_stage.colorArg2 
					);
			}

			if( current_texture_stage.alphaOp != texture_stage.alphaOp
				|| current_texture_stage.alphaArg1 != texture_stage.alphaArg1
				|| current_texture_stage.alphaArg2 != texture_stage.alphaArg2 )
			{
				current_texture_stage.alphaOp = texture_stage.alphaOp;
				current_texture_stage.alphaArg1 = texture_stage.alphaArg1;
				current_texture_stage.alphaArg2 = texture_stage.alphaArg2;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( stageId
					, current_texture_stage.alphaOp
					, current_texture_stage.alphaArg1
					, current_texture_stage.alphaArg2
					);
			}

			if( current_texture_stage.texCoordIndex != texture_stage.texCoordIndex )
			{
				current_texture_stage.texCoordIndex = texture_stage.texCoordIndex;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( stageId
					, current_texture_stage.texCoordIndex
					);
			}
		}

		if( m_alphaBlendEnable != m_currentStage->alphaBlendEnable )
		{
			m_alphaBlendEnable = m_currentStage->alphaBlendEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setAlphaBlendEnable( m_alphaBlendEnable );
		}

		if( m_currentBlendSrc != m_currentStage->blendSrc ||
			m_currentBlendDst != m_currentStage->blendDst ||
			m_currentBlendOp != m_currentStage->blendOp )
		{
			m_currentBlendSrc = m_currentStage->blendSrc;
			m_currentBlendDst = m_currentStage->blendDst;
			m_currentBlendOp = m_currentStage->blendOp;

			RENDER_SYSTEM(m_serviceProvider)
				->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateTexture_( uint32_t _stageId, const RenderTextureInterfacePtr & _texture )
	{
		uint32_t texture_id = _texture->getId();
		uint32_t current_texture_id = m_currentTexturesID[_stageId];

		if( texture_id != current_texture_id || current_texture_id != 0 )
		{
			m_currentTexturesID[_stageId] = texture_id;

			const RenderImageInterfacePtr & image = _texture->getImage();

			RENDER_SYSTEM(m_serviceProvider)
				->setTexture( _stageId, image );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateMaterial_( RenderMaterialInterface * _material )
	{		
		uint32_t materialId = _material->getId();

		if( m_currentMaterialId == materialId )
		{
			return;
		}

		m_currentMaterialId = materialId;

		uint32_t textureCount = _material->getTextureCount();

		if( m_currentTextureStages > textureCount )
		{
			for( uint32_t stageId = textureCount; stageId != m_currentTextureStages; ++stageId )
			{
				this->restoreTextureStage_( stageId );
			}
		}

		m_currentTextureStages = textureCount;

		for( uint32_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			const RenderTextureInterfacePtr & texture = _material->getTexture( stageId );

			if( texture == nullptr )
			{
				this->updateTexture_( stageId, m_nullTexture );
			}
			else
			{
				this->updateTexture_( stageId, texture );
			}
		}

		const RenderMaterialStage * stage = _material->getStage();

		this->updateStage_( stage );

		if( m_currentProgram != nullptr )
		{
			RENDER_SYSTEM( m_serviceProvider )
				->updateProgram( m_currentProgram );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject_( RenderObject * _renderObject )
	{
		if( _renderObject->dipIndiciesNum == 0 )
		{
			return;
		}

		RenderMaterialInterface * material = _renderObject->material;

		this->updateMaterial_( material );		

		if( m_currentIBHandle != _renderObject->ibHandle )
		{
			m_currentIBHandle = _renderObject->ibHandle;

			RENDER_SYSTEM(m_serviceProvider)
				->setIndexBuffer( m_currentIBHandle );
		}

		if( m_currentVBHandle != _renderObject->vbHandle )
		{
			m_currentVBHandle = _renderObject->vbHandle;

			RENDER_SYSTEM(m_serviceProvider)
				->setVertexBuffer( m_currentVBHandle );
		}

		EPrimitiveType primitiveType = material->getPrimitiveType();

		RENDER_SYSTEM(m_serviceProvider)->drawIndexedPrimitive( 
			primitiveType,
			_renderObject->baseVertexIndex, 
			_renderObject->minIndex,
			_renderObject->dipVerticesNum, 
			_renderObject->startIndex, 
			_renderObject->dipIndiciesNum 
			);

		stdex::intrusive_ptr_release( _renderObject->material );
		stdex::intrusive_ptr_release( _renderObject->vbHandle );
		stdex::intrusive_ptr_release( _renderObject->ibHandle );

		++m_debugInfo.dips;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::restoreTextureStage_( uint32_t _stage )
	{
		RenderTextureStage & stage = m_currentTextureStage[_stage];

		stage = RenderTextureStage();

		stage.mipmap = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getDefaultTextureFilterMipmap();

		stage.minification = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDefaultTextureFilterMinification();
		
		stage.magnification = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDefaultTextureFilterMagnification();

		m_currentTexturesID[_stage] = 0;

		RENDER_SYSTEM(m_serviceProvider)
			->setTexture( _stage, nullptr );

		RENDER_SYSTEM( m_serviceProvider )->setTextureAddressing( _stage
			, stage.addressU
			, stage.addressV
			);

		RENDER_SYSTEM( m_serviceProvider )->setTextureStageFilter( _stage
			, stage.minification
			, stage.mipmap
			, stage.magnification
			);
		
		RENDER_SYSTEM( m_serviceProvider )->setTextureStageColorOp( _stage
			, stage.colorOp
			, stage.colorArg1
			, stage.colorArg2
			);

		RENDER_SYSTEM( m_serviceProvider )->setTextureStageAlphaOp( _stage
			, stage.alphaOp
			, stage.colorArg1
			, stage.colorArg2
			);

		RENDER_SYSTEM( m_serviceProvider )->setTextureStageTexCoordIndex( _stage
			, stage.texCoordIndex
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::restoreRenderSystemStates_()
	{
		m_currentMaterialId = 0;
		m_currentStage = nullptr;

		m_currentBlendSrc = BF_ONE;
		m_currentBlendDst = BF_ZERO;
		m_currentBlendOp = BOP_ADD;

		m_depthBufferWriteEnable = false;
		m_alphaBlendEnable = false;

		for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			this->restoreTextureStage_( i );
		}

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );

		m_currentVBHandle = nullptr;
		m_currentIBHandle = nullptr;
		m_currentProgram = nullptr;

		RENDER_SYSTEM( m_serviceProvider )
			->setVertexBuffer( nullptr );

		RENDER_SYSTEM( m_serviceProvider )			
			->setIndexBuffer( nullptr );

		RENDER_SYSTEM( m_serviceProvider )
			->setProgram( nullptr );

		mt::mat4f viewTransform;
		mt::ident_m4( viewTransform );

		mt::mat4f projTransform;
		mt::ident_m4( projTransform );

		mt::mat4f worldTransform;
		mt::ident_m4( worldTransform );

		RENDER_SYSTEM( m_serviceProvider )->setProjectionMatrix( projTransform );
		RENDER_SYSTEM( m_serviceProvider )->setModelViewMatrix( viewTransform );
		RENDER_SYSTEM( m_serviceProvider )->setWorldMatrix( worldTransform );
		RENDER_SYSTEM( m_serviceProvider )->setCullMode( CM_CULL_NONE );
		//RENDER_SYSTEM( m_serviceProvider )->setFillMode( FM_SOLID );
		//RENDER_SYSTEM( m_serviceProvider )->setFillMode( FM_WIREFRAME );
		RENDER_SYSTEM( m_serviceProvider )->setDepthBufferTestEnable( false );
		RENDER_SYSTEM( m_serviceProvider )->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		RENDER_SYSTEM( m_serviceProvider )->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
		RENDER_SYSTEM( m_serviceProvider )->setAlphaBlendEnable( m_alphaBlendEnable );
		RENDER_SYSTEM( m_serviceProvider )->setLightingEnable( false );

		LOGGER_INFO(m_serviceProvider)("RenderEngine::restoreRenderSystemStates_ texture stages %d"
			, MENGE_MAX_TEXTURE_STAGES
			);

		RENDER_SYSTEM( m_serviceProvider )->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeProjectionOrthogonal( _projectionMatrix, _viewport, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeProjectionPerspective( _projectionMatrix, _fov, _aspect, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeProjectionFrustum( _projectionMatrix, _viewport, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeViewMatrixFromViewport( _viewMatrix, _viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeViewMatrixLookAt( _viewMatrix, _eye, _dir, _up, _sign );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const
	{
		float renderWidth = m_renderViewport.getWidth();
		float renderHeight = m_renderViewport.getHeight();

		uint32_t contentWidth = m_contentResolution.getWidth();
		uint32_t contentHeight = m_contentResolution.getHeight();

		float scale_width = renderWidth / float(contentWidth);
		float scale_height = renderHeight / float(contentHeight);
				
		Viewport renderViewport;
		renderViewport.begin.x = _viewport.begin.x * scale_width;
		renderViewport.begin.y = _viewport.begin.y * scale_height;
		renderViewport.end.x = _viewport.end.x * scale_width;
		renderViewport.end.y = _viewport.end.y * scale_height;

		renderViewport.begin += m_renderViewport.begin;
		renderViewport.end += m_renderViewport.begin;

		float vp_x = ::floorf( renderViewport.begin.x + 0.5f );
		float vp_y = ::floorf( renderViewport.begin.y + 0.5f );

		float width = renderViewport.getWidth();
		float height = renderViewport.getHeight();

		float vp_width = ::floorf( width + 0.5f );
		float vp_height = ::floorf( height + 0.5f );

		mt::vec2f windowSize;
		m_windowResolution.calcSize( windowSize );

		if( vp_x >= windowSize.x || 
			vp_y >= windowSize.y ||
			vp_x + vp_width <= 0.f || 
			vp_y + vp_height <= 0.f )
		{
			renderViewport.begin.x = 0.f;
			renderViewport.begin.y = 0.f;
			renderViewport.end.x = 0.f;
			renderViewport.end.y = 0.f;
		}
		else
		{
			if( vp_x < 0.f )
			{
				renderViewport.begin.x = 0.f;
			}

			if( vp_x + vp_width > windowSize.x )
			{
				renderViewport.end.x = windowSize.x;
			}

			if( vp_y < 0.f )
			{
				renderViewport.begin.y = 0.f;
			}

			if( vp_y + vp_height > windowSize.y )
			{
				renderViewport.end.y = windowSize.y;
			}
		}

		_renderViewport = renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderDebugInfo & RenderEngine::getDebugInfo() const
	{
		return m_debugInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::resetFrameCount()
	{
		m_debugInfo.frameCount = 0U;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPasses_()
	{
		if( m_renderPasses.empty() == true )	// nothing to render
		{
			return;
		}

		for( TArrayRenderPass::iterator
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
		it != it_end;
		++it )
		{
			RenderPass & renderPass = *it;

			this->renderPass_( renderPass );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( RenderPass & _renderPass )
	{
		if( _renderPass.viewport != nullptr )
		{
			const Viewport & viewport = _renderPass.viewport->getViewport();

			Viewport renderViewport;
			this->calcRenderViewport_( viewport, renderViewport );

			RENDER_SYSTEM( m_serviceProvider )
				->setViewport( renderViewport );
		}
		else
		{
			uint32_t width = m_contentResolution.getWidth();
			uint32_t height = m_contentResolution.getHeight();

			Viewport renderViewport;
			renderViewport.begin.x = 0.f;
			renderViewport.begin.y = 0.f;
			renderViewport.end.x = (float)width;
			renderViewport.end.y = (float)height;

			RENDER_SYSTEM( m_serviceProvider )
				->setViewport( renderViewport );
		}

		if( _renderPass.clipplane != nullptr )
		{
			uint32_t count = _renderPass.clipplane->getCount();

			RENDER_SYSTEM( m_serviceProvider )
				->setClipplaneCount( count );

			for( uint32_t i = 0; i != count; ++i )
			{
				const mt::planef & p = _renderPass.clipplane->getPlane( i );

				RENDER_SYSTEM( m_serviceProvider )
					->setClipplane( i, p );
			}
		}
		else
		{
			RENDER_SYSTEM( m_serviceProvider )
				->setClipplaneCount( 0 );
		}

		if( _renderPass.camera != nullptr )
		{
			//const mt::mat4f & worldMatrix = _renderPass.camera->getCameraWorldMatrix();

			//RENDER_SYSTEM( m_serviceProvider )
			//	->setWorldMatrix( worldMatrix );

			const mt::mat4f & viewMatrix = _renderPass.camera->getCameraViewMatrix();

			RENDER_SYSTEM( m_serviceProvider )
				->setModelViewMatrix( viewMatrix );

			const mt::mat4f & projectionMatrix = _renderPass.camera->getCameraProjectionMatrix();

			RENDER_SYSTEM( m_serviceProvider )
				->setProjectionMatrix( projectionMatrix );
		}
		else
		{
			//mt::mat4f worldMatrix;
			//mt::ident_m4( worldMatrix );

			//RENDER_SYSTEM( m_serviceProvider )
			//	->setWorldMatrix( worldMatrix );

			mt::mat4f viewMatrix;
			mt::ident_m4( viewMatrix );

			RENDER_SYSTEM( m_serviceProvider )
				->setModelViewMatrix( viewMatrix );

			mt::mat4f projectionMatrix;
			mt::ident_m4( projectionMatrix );

			RENDER_SYSTEM( m_serviceProvider )
				->setProjectionMatrix( projectionMatrix );
		}

		this->renderObjects_( _renderPass );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObjects_( RenderPass & _renderPass )
	{
		TArrayRenderObject::iterator it_begin = m_renderObjects.advance( _renderPass.beginRenderObject );
		TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass.beginRenderObject + _renderPass.countRenderObject );

		for( ;it_begin != it_end; ++it_begin )
		{
			RenderObject * renderObject = it_begin;

			this->renderObject_( renderObject );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::createRenderPass_()
	{
		RenderPass & pass = m_renderPasses.emplace();

		pass.beginRenderObject = (uint32_t)m_renderObjects.size();
		pass.countRenderObject = 0U;
		pass.viewport = m_currentRenderViewport;
		pass.camera = m_currentRenderCamera;
		pass.clipplane = m_currentRenderClipplane;
		pass.target = m_currentRenderTarget;

		for( uint32_t i = 0U; i != MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX; ++i )
		{
			pass.materialEnd[i] = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderObject( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
		, const RenderVertex2D * _vertices, uint32_t _verticesNum
		, const RenderIndices * _indices, uint32_t _indicesNum
		, const mt::box2f * _bb, bool _debug )
	{
#	ifdef _DEBUG
		if( _state == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::renderObject2D _state == NULL"
				);

			return;
		}

		if( _state->viewport == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D viewport == NULL"
				);

			return;
		}

		if( _state->camera == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D camera == NULL"
				);

			return;
		}

		if( _material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D _material == NULL"
				);

			return;
		}

		if( _vertices == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::renderObject2D _vertices == NULL"
				);

			return;
		}

		if( _indices == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::renderObject2D _indices == NULL"
				);

			return;
		}
#	endif

		if( m_renderObjects.full() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::renderObject2D max render objects %u"
				, m_renderObjects.size()
				);

			return;
		}

		if( m_currentRenderViewport != _state->viewport || 
			m_currentRenderCamera != _state->camera ||			
			m_currentRenderClipplane != _state->clipplane || 
			m_currentRenderTarget != _state->target )
		{
			if( m_renderPasses.full() == true )
			{
				LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D max render passes %u"
					, m_renderPasses.size()
					);

				return;
			}

			m_currentRenderViewport = _state->viewport;
			m_currentRenderCamera = _state->camera;			
			m_currentRenderClipplane = _state->clipplane;
			m_currentRenderTarget = _state->target;

			this->createRenderPass_();
		}

		RenderPass & rp = m_renderPasses.back();

		mt::box2f bb;

		if( _bb != nullptr )
		{
			bb = *_bb;
		}
		else
		{
			Helper::makeRenderBoundingBox( bb, _vertices, _verticesNum );
		}
		
		const mt::mat4f & vpm = rp.camera->getCameraViewProjectionMatrix();

		const Viewport & vp = rp.viewport->getViewport();

		mt::box2f bb_homogenize;
		mt::set_box_homogenize( bb_homogenize, bb.minimum, bb.maximum, vpm );

		mt::vec2f vp_scale;
		vp.calcSize( vp_scale );

		mt::scale_box( bb_homogenize, vp_scale );
		mt::transpose_box( bb_homogenize, vp.begin );

		RenderMaterialInterfacePtr ro_material = _material;

		if( m_debugStepRenderMode == true && _debug == false )
		{
			if( m_iterateRenderObjects++ >= m_limitRenderObjects && m_limitRenderObjects > 0 && m_stopRenderObjects == false )
			{
				return;
			}

			EPrimitiveType primitiveType = ro_material->getPrimitiveType();

			switch( primitiveType )
			{
			case PT_TRIANGLELIST:
				{
					this->calcQuadSquare_( _vertices, _verticesNum );
				}break;
			default:
				{
				}break;
			}

			if( m_iterateRenderObjects == m_limitRenderObjects && m_limitRenderObjects > 0 && m_stopRenderObjects == false )
			{				
				RenderMaterialPtr new_material = RENDERMATERIAL_SERVICE(m_serviceProvider)
					->getMaterial( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Color_Blend")
					, ro_material->getPrimitiveType()
					, 0
					, nullptr
					);

				if( new_material == nullptr )
				{
					return;
				}

				ro_material = new_material;
			}
		}

		if( m_debugRedAlertMode == true && _debug == false )
		{
			RenderMaterialInterfacePtr new_material = RENDERMATERIAL_SERVICE( m_serviceProvider )
				->getMaterial( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Color_Blend" )
				, ro_material->getPrimitiveType()
				, 0
				, nullptr
				);

			if( new_material == nullptr )
			{
				return;
			}

			ro_material = new_material;
		}

		++rp.countRenderObject;

		RenderObject & ro = m_renderObjects.emplace();

		stdex::intrusive_ptr_setup( ro.material, ro_material );
		
		uint32_t materialId = ro.material->getId();
		ro.materialSmartId = materialId % MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX;

		rp.materialEnd[ro.materialSmartId] = &ro;

		stdex::intrusive_ptr_setup( ro.ibHandle, m_ibHandle2D );
		stdex::intrusive_ptr_setup( ro.vbHandle, m_vbHandle2D );

		ro.vertexData = _vertices;
		ro.vertexCount = _verticesNum;

		ro.indicesData = _indices;
		ro.indicesCount = _indicesNum;

		ro.bb = bb_homogenize;

		ro.minIndex = 0;
		ro.startIndex = 0;
		ro.baseVertexIndex = 0;

		ro.dipVerticesNum = 0;
		ro.dipIndiciesNum = 0;
		ro.flags = 0;

		if( _debug == true )
		{
			ro.flags |= RO_FLAG_DEBUG;
		}
		
		m_renderVertexCount += _verticesNum;
		m_renderIndicesCount += _indicesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderQuad( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
		, const RenderVertex2D * _vertices, uint32_t _verticesNum
		, const mt::box2f * _bb, bool _debug )
	{
		uint32_t indicesNum = (_verticesNum / 4) * 6;

		if( indicesNum >= m_indicesQuad.size() )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderQuad count %d > max count %d"
				, indicesNum
				, m_indicesQuad.size()
				);

			return;
		}

		RenderIndices * indices = m_indicesQuad.buff();

		this->addRenderObject( _state, _material, _vertices, _verticesNum, indices, indicesNum, _bb, _debug );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderLine( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
		, const RenderVertex2D * _vertices, uint32_t _verticesNum
		, const mt::box2f * _bb, bool _debug )
	{
		uint32_t indicesNum = _verticesNum;

		if( indicesNum >= m_indicesLine.size() )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderLine count %d > max count %d"
				, indicesNum
				, m_indicesLine.size()
				);

			return;
		}

		RenderIndices * indices = m_indicesLine.buff();

		this->addRenderObject( _state, _material, _vertices, _verticesNum, indices, indicesNum, _bb, _debug );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertex2D * RenderEngine::getDebugRenderVertex2D( size_t _count )
	{
		m_debugVertices.push_back( TArrayRenderVertex2D() );
		TArrayRenderVertex2D & vertices_array = m_debugVertices.back();
		vertices_array.resize( _count );

		RenderVertex2D * vertices = &vertices_array.front();

		return vertices;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setBatchMode( ERenderBatchMode _mode )
	{
		m_batchMode = _mode;
	}
	//////////////////////////////////////////////////////////////////////////
	ERenderBatchMode RenderEngine::getBatchMode() const
	{
		return m_batchMode;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertexBufferInterfacePtr RenderEngine::createVertexBuffer( const RenderVertex2D * _buffer, uint32_t _count )
	{
		RenderVertexBufferInterfacePtr vb = RENDER_SYSTEM( m_serviceProvider )
			->createVertexBuffer( _count, false );

		if( vb == nullptr )
		{
			return nullptr;
		}

		this->updateVertexBuffer( vb, _buffer, _count );

		return vb;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndexBufferInterfacePtr RenderEngine::createIndicesBuffer( const RenderIndices * _buffer, uint32_t _count )
	{
		RenderIndexBufferInterfacePtr ib = RENDER_SYSTEM( m_serviceProvider )
			->createIndexBuffer( _count, false );

		if( ib == nullptr )
		{
			return nullptr;
		}

		this->updateIndicesBuffer( ib, _buffer, _count );

		return ib;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::updateVertexBuffer( const RenderVertexBufferInterfacePtr & _vb, const RenderVertex2D * _buffer, uint32_t _count )
	{
		RenderVertex2D * vbuffer = _vb->lock( 0, _count, BLF_LOCK_DISCARD );

		if( vbuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateVertexBuffer failed to lock vertex buffer"
				);

			return false;
		}

		stdex::memorycopy_pod( vbuffer, 0, _buffer, _count );
		
		if( _vb->unlock() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateVertexBuffer failed to unlock vertex buffer" 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::updateIndicesBuffer( const RenderIndexBufferInterfacePtr & _ib, const RenderIndices * _buffer, uint32_t _count )
	{
		RenderIndices * ibuffer = _ib->lock( 0, _count, BLF_LOCK_DISCARD );

		if( ibuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer failed to lock vertex buffer"
				);

			return false;
		}

		stdex::memorycopy_pod( ibuffer, 0, _buffer, _count );

		if( _ib->unlock() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to unlock vertex buffer"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::makeBatches_()
	{		
		if( m_renderVertexCount >= m_maxVertexCount )
		{
			LOGGER_WARNING(m_serviceProvider)("RenderEngine::makeBatches_: vertex buffer overflow"
				);

			return false;
		}

		if( m_renderIndicesCount >= m_maxIndexCount )
		{
			LOGGER_WARNING(m_serviceProvider)("RenderEngine::makeBatches_: indices buffer overflow"
				);

			return false;
		}

		if( m_renderVertexCount == 0 )
		{
			return false;
		}

		if( m_renderIndicesCount == 0 )
		{
			return false;
		}
				
		RenderVertex2D * vertexBuffer = m_vbHandle2D->lock( 0, m_renderVertexCount, BLF_LOCK_DISCARD );

		if( vertexBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock vertex buffer"
				);

			return false;
		}

		RenderIndices * indicesBuffer = m_ibHandle2D->lock( 0, m_renderIndicesCount, BLF_LOCK_DISCARD );

		if( indicesBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock indices buffer"
				);

			return false;
		}

		this->insertRenderPasses_( vertexBuffer, indicesBuffer, m_renderVertexCount, m_renderIndicesCount );

		if( m_ibHandle2D->unlock() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to unlock indices buffer"
				);

			return false;
		}

		if( m_vbHandle2D->unlock() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to unlock vertex buffer"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderPasses_( RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize )
	{
		uint32_t vbPos = 0;
		uint32_t ibPos = 0;

		for( TArrayRenderPass::iterator 
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
		it != it_end;
		++it )
		{
			RenderPass * pass = &(*it);

			this->insertRenderObjects_( pass, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::batchRenderObjectNormal_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos )
	{
		uint32_t vbPos = _vbPos;
		uint32_t ibPos = _ibPos;

		TArrayRenderObject::iterator it_batch_begin = _begin;
		++it_batch_begin;

		RenderMaterialInterface * ro_material = _ro->material;

		for( ; it_batch_begin != _end; ++it_batch_begin )
		{
			RenderObject * ro_bath_begin = it_batch_begin;

			if( ro_bath_begin->indicesCount == 0 )
			{
				continue;
			}

			RenderMaterialInterface * batch_material = ro_bath_begin->material;

			if( ro_material != batch_material )
			{
				break;
			}

			if( this->insertRenderObject_( ro_bath_begin, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos ) == false )
			{
				break;
			}

			vbPos += ro_bath_begin->vertexCount;
			ibPos += ro_bath_begin->indicesCount;

			_ro->dipVerticesNum += ro_bath_begin->vertexCount;
			_ro->dipIndiciesNum += ro_bath_begin->indicesCount;

			stdex::intrusive_ptr_release( ro_bath_begin->material );
			stdex::intrusive_ptr_release( ro_bath_begin->vbHandle );
			stdex::intrusive_ptr_release( ro_bath_begin->ibHandle );

			ro_bath_begin->dipVerticesNum = 0;
			ro_bath_begin->dipIndiciesNum = 0;
			ro_bath_begin->vertexCount = 0;
			ro_bath_begin->indicesCount = 0;

			++m_debugInfo.batch;
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_testRenderBB( const RenderObject * _begin, const RenderObject * _end, const RenderObject * _ro )
	{
		for( const RenderObject * it = _begin; it != _end; ++it )
		{
			const RenderObject * ro_bath = it;

			if( ro_bath->vertexCount == 0 )
			{
				continue;
			}

			if( mt::is_intersect( ro_bath->bb, _ro->bb ) == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::batchRenderObjectSmart_( RenderPass * _renderPass, TArrayRenderObject::iterator _begin, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos )
	{
		uint32_t vbPos = _vbPos;
		uint32_t ibPos = _ibPos;

		TArrayRenderObject::iterator it_batch_start_end = _begin;
		++it_batch_start_end;

		TArrayRenderObject::iterator it_batch = _begin;
		++it_batch;

		uint32_t materialSmartId = _ro->materialSmartId;
		TArrayRenderObject::const_iterator it_end = _renderPass->materialEnd[materialSmartId];

		if( _begin == it_end )
		{
			return;
		}

		const RenderMaterialInterface * ro_material = _ro->material;

		for( ; it_batch != it_end; ++it_batch )
		{
			RenderObject * ro_bath = it_batch;

			if( ro_bath->vertexCount == 0 )
			{
				continue;
			}

			const RenderMaterialInterface * ro_bath_material = ro_bath->material;

			if( ro_material != ro_bath_material )
			{
				continue;
			}
			
			if( s_testRenderBB( it_batch_start_end, it_batch, ro_bath ) == true )
			{
				break;
			}

			if( this->insertRenderObject_( ro_bath, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos ) == false )
			{
				break;
			}

			_ro->dipVerticesNum += ro_bath->vertexCount;
			_ro->dipIndiciesNum += ro_bath->indicesCount;

			stdex::intrusive_ptr_release( ro_bath->material );
			stdex::intrusive_ptr_release( ro_bath->vbHandle );
			stdex::intrusive_ptr_release( ro_bath->ibHandle );

			ro_bath->dipVerticesNum = 0;
			ro_bath->dipIndiciesNum = 0;

			vbPos += ro_bath->vertexCount;
			ibPos += ro_bath->indicesCount;

			ro_bath->vertexCount = 0;
			ro_bath->indicesCount = 0;

			++m_debugInfo.batch;
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderObjects_( RenderPass * _renderPass, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos )
	{
		uint32_t vbPos = _vbPos;
		uint32_t ibPos = _ibPos;

		TArrayRenderObject::iterator it = m_renderObjects.advance( _renderPass->beginRenderObject );
		TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );
				
		for( ; it != it_end; ++it )
		{
			RenderObject * ro = it;

			if( ro->vertexCount == 0 )
			{
				continue;
			}
			
			ro->startIndex = ibPos;
			ro->minIndex = vbPos;

			if( this->insertRenderObject_( ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos ) == false )
			{
				break;
			}
			
			ro->dipVerticesNum = ro->vertexCount;
			ro->dipIndiciesNum = ro->indicesCount;

			vbPos += ro->vertexCount;
			ibPos += ro->indicesCount;

			ro->vertexCount = 0;
			ro->indicesCount = 0;

			switch( m_batchMode )
			{
			case ERBM_NONE:
				{
				}break;
			case ERBM_NORMAL:
				{
					this->batchRenderObjectNormal_( it, it_end, ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
				}break;
			case ERBM_SMART:
				{
					this->batchRenderObjectNormal_( it, it_end, ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );

					this->batchRenderObjectSmart_( _renderPass, it, ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
				}break;
			}	
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::insertRenderObject_( const RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t _vbPos, uint32_t _ibPos ) const
	{   
		if( stdex::memorycopy_safe_pod( _vertexBuffer, _vbPos, _vbSize, _renderObject->vertexData, _renderObject->vertexCount ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::insertRenderObject_ vertex buffer overrlow!"
				);

			return false;
		}

		if( m_debugRedAlertMode == true && hasRenderObjectFlag( _renderObject, RO_FLAG_DEBUG ) == false )
		{
			for( uint32_t i = 0; i != _renderObject->vertexCount; ++i )
			{
				RenderVertex2D & v = _vertexBuffer[_vbPos + i];

				v.color = Helper::makeARGB( 1.f, 0.f, 0.f, 0.1f );
			}
		}
		
		if( _ibPos > _ibSize )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::insertRenderObject_ indices buffer overrlow!"
				);
			
			return false;
		}

		RenderIndices * offsetIndicesBuffer = _indicesBuffer + _ibPos;

		RenderIndices * src = offsetIndicesBuffer;
		RenderIndices * src_end = offsetIndicesBuffer + _renderObject->indicesCount;
		const RenderIndices * dst = _renderObject->indicesData;

		RenderIndices indices_offset = (RenderIndices)_vbPos;
		while( src != src_end )
		{
			*src = *dst + indices_offset;
			++src;
			++dst;
		}

		//_renderObject->baseVertexIndex = 0;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::flushRender_()
	{
		m_debugInfo.batch = 0;
		m_debugInfo.dips = 0;

		if( this->makeBatches_() == false )
		{
			return;
		}

		this->renderPasses_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::create2DBuffers_()
	{
		m_ibHandle2D = RENDER_SYSTEM(m_serviceProvider)
			->createIndexBuffer( m_maxIndexCount, false );

		if( m_ibHandle2D == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		m_vbHandle2D = RENDER_SYSTEM(m_serviceProvider)
			->createVertexBuffer( m_maxVertexCount, true );

		if( m_vbHandle2D == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setVSync( bool _vSync )
	{
		if( m_vsync == _vSync )
		{
			return;
		}

		m_vsync = _vSync;

		if( m_windowCreated == true )
		{
			RENDER_SYSTEM(m_serviceProvider)
				->setVSync( m_vsync );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::clear( uint32_t _color )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->clear( _color, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setSeparateAlphaBlendMode()
	{
		RENDER_SYSTEM(m_serviceProvider)
			->setSeparateAlphaBlendMode();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableDebugStepRenderMode( bool _enable )
	{
		m_debugStepRenderMode = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isDebugStepRenderMode() const
	{
		return m_debugStepRenderMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableRedAlertMode( bool _enable )
	{
		m_debugRedAlertMode = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isRedAlertMode() const
	{
		return m_debugRedAlertMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLimitRenderObjects()
	{
		m_stopRenderObjects = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::increfLimitRenderObjects()
	{
		++m_limitRenderObjects;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::decrefLimitRenderObjects()
	{
		if( m_limitRenderObjects == 0 )
		{
			return false;
		}

		--m_limitRenderObjects;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & RenderEngine::getRenderViewport() const
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	static double s_calcTriangleSquare( const RenderVertex2D & _v1, const RenderVertex2D & _v2, const RenderVertex2D & _v3 )
	{
		const mt::vec3f & p1 = _v1.position;
		const mt::vec3f & p2 = _v2.position;
		const mt::vec3f & p3 = _v3.position;

		double a = (double)mt::length_v3_v3( p1, p2 );
		double b = (double)mt::length_v3_v3( p2, p3 );
		double c = (double)mt::length_v3_v3( p3, p1 );

		double p = (a + b + c) * 0.5;

		double sq_p = sqrt(p);
		double sq_pa = sqrt(fabs(p - a));
		double sq_pb = sqrt(fabs(p - b));
		double sq_pc = sqrt(fabs(p - c));

		double S = sq_p * sq_pa * sq_pb * sq_pc;

		return S;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcQuadSquare_( const RenderVertex2D * _vertex, uint32_t _num )
	{
		for( uint32_t i = 0; i != (_num / 4); ++i )
		{
			const RenderVertex2D & v0 = _vertex[i*4 + 0];
			const RenderVertex2D & v1 = _vertex[i*4 + 1];
			const RenderVertex2D & v2 = _vertex[i*4 + 2];
			const RenderVertex2D & v3 = _vertex[i*4 + 3];

			m_debugInfo.fillrate += s_calcTriangleSquare( v0, v1, v2 );
			m_debugInfo.fillrate += s_calcTriangleSquare( v0, v2, v3 );

			m_debugInfo.triangle += 2;
		}        

		m_debugInfo.object += 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcMeshSquare_( const RenderVertex2D * _vertex, uint32_t _verteNum, const RenderIndices * _indices, uint32_t _indicesNum )
	{
		(void)_verteNum;

		for( uint32_t i = 0; i != (_indicesNum / 3); ++i )
		{
			RenderIndices i0 = _indices[i + 0];
			RenderIndices i1 = _indices[i + 1];
			RenderIndices i2 = _indices[i + 2];

			m_debugInfo.fillrate += s_calcTriangleSquare( _vertex[i0], _vertex[i1], _vertex[i2] );

			m_debugInfo.triangle += 1;
		}

		m_debugInfo.object += 1;
	}
}
