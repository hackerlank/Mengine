#	pragma once

#	include "Interface/TextInterface.h"

#	include "Factory/Factorable.h"

#	include "stdex/stl_map.h"

#	include "stdex/intrusive_tree.h"

namespace Menge
{
	class TextGlyphChar
		: public Factorable
		, public stdex::intrusive_tree_node<TextGlyphChar>
	{
	public:
		typedef GlyphCode key_type;	
		typedef GlyphCharLess less_type;

		struct key_getter_type
		{
			const GlyphCode & operator()( const TextGlyphChar * _node ) const
			{
				return _node->m_code;
			}
		};

	public:
		TextGlyphChar();
		
	public:
		void initialize( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size );

	public:
		inline const mt::vec4f & getUV() const;
		inline const mt::vec2f & getOffset() const;
		inline float getAdvance() const;
		inline const mt::vec2f & getSize() const;

	public:
		void addKerning( GlyphCode _char, float _kerning );
		float getKerning( GlyphCode _char ) const;

	protected:
		GlyphCode m_code;

		mt::vec4f m_uv;
		mt::vec2f m_offset;
		float m_advance;
		mt::vec2f m_size;

		typedef stdex::map<GlyphCode, float, GlyphCharLess> TMapKerning;
		TMapKerning m_kernings;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec4f & TextGlyphChar::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & TextGlyphChar::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float TextGlyphChar::getAdvance() const
	{
		return m_advance;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & TextGlyphChar::getSize() const
	{
		return m_size;
	}
}