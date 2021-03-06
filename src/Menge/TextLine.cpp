#	include "TextLine.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#   include <utf8.h>

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLine::TextLine( ServiceProviderInterface * _serviceProvider, float _charOffset )
		: m_serviceProvider(_serviceProvider)
		, m_length(0.f)
        , m_charOffset(_charOffset)
		, m_offset(0.f)
		, m_invalidateTextLine(true)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextLine::initialize( const TextFontInterfacePtr & _font, const String & _text )
	{
		WString::size_type text_size = _text.length();
		m_charsData.reserve( text_size );
				
        const char * text_str = _text.c_str();
        size_t text_len = _text.size();

        //const char * text_it = text_str;
        //const char * text_end = text_str + text_len + 1;

		const RenderTextureInterfacePtr & textureFont = _font->getTextureFont();

		float image_width = (float)textureFont->getWidth();
		float image_height = (float)textureFont->getHeight();

		float inv_image_width = 1.f / image_width;
		float inv_image_height = 1.f / image_height;

		bool successful = true;

		for( const char
            *text_it = text_str,
            *text_end = text_str + text_len + 1;
        text_it != text_end;
        )
		{			
			uint32_t code = 0;
			utf8::internal::utf_error err = utf8::internal::validate_next( text_it, text_end, code );
			
			if( err != utf8::internal::UTF8_OK )
			{
				LOGGER_ERROR(m_serviceProvider)("TextLine for fontName %s invalid glyph |%s| err code %d"
					, _font->getName().c_str()
					, text_it
					, err
					);

				successful = false;

				continue;
			}

            if( code == 0 )
            {
                continue;
            }			
			else if( code == 160 )
			{
				//'nbsp' replace to space :)
				code = 32;
			}
			else if( code == 9 )
			{
				code = 32;
			}

			GlyphCode glyphChar;
			glyphChar.setCode( code );

			uint32_t code_next = 0;
			const char * text_it_next = text_it;		
			utf8::internal::utf_error err_next = utf8::internal::validate_next( text_it_next, text_end, code_next );

			if( err_next != utf8::internal::UTF8_OK )
			{
				LOGGER_ERROR(m_serviceProvider)("TextLine for fontName %s invalid glyph |%s| err code %d"
					, _font->getName().c_str()
					, text_it
					, err
					);

				successful = false;

				continue;
			}

            GlyphCode glyphCharNext;
            glyphCharNext.setCode( code_next );

			Glyph glyph;
			if( _font->getGlyph( glyphChar, glyphCharNext, &glyph ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextLine for fontName %s invalid glyph %d next %d"
					, _font->getName().c_str()
					, code
					, code_next
					);

				glyph.uv = mt::vec4f( 0.f, 0.f, 0.f, 0.f );
				glyph.offset = mt::vec2f( 0.f, 0.f );
				glyph.advance = 0.f;
				glyph.size = mt::vec2f( 0.f, 0.f );

				successful = false;

				continue;
			}			
					
			CharData charData;

			charData.code = glyphChar;	

			mt::ident_v2( charData.vertex[0] );
			mt::ident_v2( charData.vertex[1] );
			mt::ident_v2( charData.vertex[2] );
			mt::ident_v2( charData.vertex[3] );

			charData.uv = glyph.uv;

			charData.uv.x *= inv_image_width;
			charData.uv.y *= inv_image_height;
			charData.uv.z *= inv_image_width;
			charData.uv.w *= inv_image_height;

			charData.advance = glyph.advance;
			charData.offset = glyph.offset;
			
			charData.size = glyph.size;

			m_charsData.push_back( charData );

			m_length += charData.advance + m_charOffset;
		}

		m_length -= m_charOffset;

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t TextLine::getCharsDataSize() const
	{
		return (uint32_t)m_charsData.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextLine::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::prepareRenderObject(	mt::vec2f & _offset		
		, float _charScale
		, const mt::uv4f & _uv
		, ColourValue_ARGB _argb
		, TVectorRenderVertex2D & _renderObject ) const
	{
		if( m_invalidateTextLine == true )
		{
			this->updateRenderLine_( _offset, _charScale );
		}

		size_t renderObjectNum = _renderObject.size();

		size_t chars_size = m_charsData.size();

		_renderObject.resize( renderObjectNum + chars_size * 4 );

		for( TVectorCharData::const_iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			const CharData & data = *it_char;

			for( uint32_t i = 0; i != 4; ++i )
			{
				RenderVertex2D & renderVertex = _renderObject[renderObjectNum + i];

				const mt::vec2f & charVertex = data.vertex[i];

				renderVertex.position.x = charVertex.x;
				renderVertex.position.y = charVertex.y;

				renderVertex.position.z = 0.f;

				renderVertex.color = _argb;
			}

			const mt::vec4f & char_uv = data.uv;

			mt::uv4f total_uv;
			mt::multiply_tetragon_uv4_v4( total_uv, _uv, char_uv );

			for( size_t i = 0; i != 4; ++i )
			{
				_renderObject[renderObjectNum + i].uv[0] = total_uv[i];
				_renderObject[renderObjectNum + i].uv[1] = total_uv[i];
			}

			renderObjectNum += 4;
		}

		_offset.x += m_offset;

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateRenderLine_( mt::vec2f & _offset, float _charScale ) const
	{
		m_invalidateTextLine = false;

		for( TVectorCharData::iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			CharData & cd = *it_char;

			mt::vec2f size = cd.size * _charScale;

			mt::vec2f offset = _offset + cd.offset * _charScale;
			
			cd.vertex[0] = offset + mt::vec2f( 0.f, 0.f );
			cd.vertex[1] = offset + mt::vec2f( size.x, 0.f );
			cd.vertex[2] = offset + mt::vec2f( size.x, size.y );
			cd.vertex[3] = offset + mt::vec2f( 0.f, size.y );

			_offset.x += (cd.advance + m_charOffset) * _charScale;
		}

		m_offset = _offset.x;		
	}
	//////////////////////////////////////////////////////////////////////////
}
