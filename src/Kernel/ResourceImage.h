#	pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ImageCodecInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Core/ColourValue.h"

#	include "Math/vec4.h"
#	include "Math/uv4.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceImage
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceImage();
		~ResourceImage();
			
	public:        
		inline void setMaxSize( const mt::vec2f & _size );
		inline const mt::vec2f & getMaxSize() const;

		inline void setSize( const mt::vec2f & _size );
		inline const mt::vec2f & getSize() const;

		inline void setOffset( const mt::vec2f & _size );
		inline const mt::vec2f & getOffset() const;

		inline void setUVImage( const mt::uv4f & _uv );
		inline const mt::uv4f & getUVImage() const;

		inline void setUVAlpha( const mt::uv4f & _uv );
        inline const mt::uv4f & getUVAlpha() const;

		inline void setAlpha( bool _alpha );
		inline bool isAlpha() const;

		inline void setPremultiply( bool _alpha );
		inline bool isPremultiply() const;
    
    public:
		inline const RenderTextureInterfacePtr & getTexture() const;
        inline const RenderTextureInterfacePtr & getTextureAlpha() const;

		inline void setColor( const ColourValue & _colour );
		inline const ColourValue & getColor() const;
        
    protected:
        void _release() override;

    public:
		size_t getMemoryUse() const override;
		
	protected:        
        RenderTextureInterfacePtr m_texture;
        RenderTextureInterfacePtr m_textureAlpha;

		ColourValue m_textureColor;

        mt::vec2f m_maxSize;
        mt::vec2f m_size;
		mt::vec2f m_offset;
        
		mt::uv4f m_uv_image;
        mt::uv4f m_uv_alpha;

        bool m_isAlpha;
		bool m_isPremultiply;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceImage> ResourceImagePtr;
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setMaxSize( const mt::vec2f & _maxSize )
	{
		m_maxSize = _maxSize;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getMaxSize() const
    {
        return m_maxSize;
    }
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setSize( const mt::vec2f & _size )
	{
		m_size = _size;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getSize() const
    {
        return m_size;
    }
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setOffset( const mt::vec2f & _offset )
	{
		m_offset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & ResourceImage::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setUVImage( const mt::uv4f & _uv )
	{
		m_uv_image = _uv;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::uv4f & ResourceImage::getUVImage() const
    {
        return m_uv_image;
    }
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setUVAlpha( const mt::uv4f & _uv )
	{
		m_uv_alpha = _uv;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::uv4f & ResourceImage::getUVAlpha() const
    {
        return m_uv_alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const RenderTextureInterfacePtr & ResourceImage::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const RenderTextureInterfacePtr & ResourceImage::getTextureAlpha() const
    {
        return m_textureAlpha;
    }
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setColor( const ColourValue & _colour )
	{
		m_textureColor = _colour;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ColourValue & ResourceImage::getColor() const
	{
		return m_textureColor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setAlpha( bool _alpha )
	{
		m_isAlpha = _alpha;
	}
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isAlpha() const
    {
        return m_isAlpha;
    }
	//////////////////////////////////////////////////////////////////////////
	inline void ResourceImage::setPremultiply( bool _premultiply )
	{
		m_isPremultiply = _premultiply;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool ResourceImage::isPremultiply() const
	{
		return m_isPremultiply;
	}
}
