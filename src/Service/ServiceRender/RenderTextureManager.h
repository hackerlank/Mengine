#   pragma once

#   include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "RenderTexture.h"

#	include "Core/ConstString.h"

#   include "Factory/FactoryStore.h"
#	include "Factory/FactoryDefault.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define MENGINE_TEXTURE_MANAGER_HASH_SIZE 4096
	//////////////////////////////////////////////////////////////////////////
    class RenderTextureManager
        : public ServiceBase<RenderTextureServiceInterface>
    {
    public:
        RenderTextureManager();
        ~RenderTextureManager();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codec ) override;
		RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels ) override;

	protected:
		ImageDecoderInterfacePtr createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec );
		RenderTextureInterfacePtr createTextureFromDecoder_( const ImageDecoderInterfacePtr & _decoder );

    public:
        RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;

	public:
        RenderTextureInterfacePtr getTexture( const ConstString& _pakName, const FilePath & _fileName ) const override;

    public:
        bool hasTexture( const ConstString& _pakName, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const override;

    public:
        void imageQuality( const RenderTextureInterfacePtr & _texture, void * _textureBuffer, size_t _texturePitch ) override;

        void cacheFileTexture( const ConstString& _pakName, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileGroupName, const ConstString & _codecName, const FilePath & _fileName ) override;

    public:
        void visitTexture( VisitorRenderTextureInterface * _visitor ) const override;
		size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const override;

    public:
        const RenderTextureDebugInfo & getDebugInfo() override;

	protected:
        bool onRenderTextureDestroy_( RenderTextureInterface * _texture );				
		void releaseRenderTexture_( RenderTextureInterface * _texture );

    protected:
        bool loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder );

	protected:
		void updateImageParams_( uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, PixelFormat & _format ) const;

    protected:
		typedef std::pair<ConstString, FilePath> TMapRenderTextureKey;
		typedef stdex::map<TMapRenderTextureKey, RenderTextureInterface *> TMapRenderTextureEntry;
		TMapRenderTextureEntry m_textures[MENGINE_TEXTURE_MANAGER_HASH_SIZE];

        typedef FactoryPoolStore<RenderTexture, 128> TFactoryRenderTexture;
        TFactoryRenderTexture m_factoryRenderTexture;

        uint32_t m_textureEnumerator;

        RenderTextureDebugInfo m_debugInfo;

		bool m_supportA8;
		bool m_supportR8G8B8;
		bool m_supportNonPow2;
    };
}