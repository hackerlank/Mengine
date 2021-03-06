#   pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/RenderSystemInterface.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"
#   include "Core/GlyphChar.h"
#   include "Core/ColourValue.h"

#   include "Factory/FactorablePtr.h"

#	include "Math/vec4.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	enum ETextHorizontAlign
	{
		ETFHA_LEFT = 0,
		ETFHA_CENTER = 1,
		ETFHA_RIGHT = 2
	};
	//////////////////////////////////////////////////////////////////////////
	enum ETextVerticalAlign
	{
		ETFVA_BOTTOM = 0,
		ETFVA_CENTER = 1,
		ETFVA_TOP = 2,
	};
	//////////////////////////////////////////////////////////////////////////
	enum EFontParams
	{
		EFP_NONE = 0x00000000,
		EFP_FONT = 0x00000001,
		EFP_COLOR_FONT = 0x00000002,
		EFP_COLOR_OUTLINE = 0x00000004,
		EFP_LINE_OFFSET = 0x00000008,
		EFP_CHAR_OFFSET = 0x00000010,
		EFP_MAX_LENGTH = 0x00000020,
		EFP_HORIZONTAL_ALIGN = 0x00000040,
		EFP_VERTICAL_ALIGN = 0x00000080,
		EFP_CHAR_SCALE = 0x00000100,
		EFP_MAX_VALUE = 0xffffffff
	};
	//////////////////////////////////////////////////////////////////////////
	struct Glyph
	{
		mt::vec4f uv;
		mt::vec2f offset;		
		mt::vec2f size;

		float advance;
	};
	//////////////////////////////////////////////////////////////////////////
	class TextFontInterface
		: public FactorablePtr
	{
	public:
		virtual const ConstString & getName() const = 0;

	public:
		virtual bool hasGlyph( GlyphCode _char ) const = 0;
		virtual bool getGlyph( GlyphCode _char, GlyphCode _prev, Glyph * _glyph ) const = 0;

	public:
		virtual float getFontHeight() const = 0;

	public:
		virtual uint32_t getFontParams() const = 0;

		virtual const ColourValue & getColorFont() const = 0;
		virtual const ColourValue & getColorOutline() const = 0;

		virtual float getLineOffset() const = 0;
		virtual float getCharOffset() const = 0;

	public:
		virtual const RenderTextureInterfacePtr & getTextureFont() const = 0;
		virtual const RenderTextureInterfacePtr & getTextureOutline() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextFontInterface> TextFontInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class TextEntryInterface
		: public Factorable
	{
	public:
		virtual const ConstString & getKey() const = 0;
		virtual const ConstString & getValue() const = 0;

	public:
		virtual const ConstString & getFontName() const = 0;
		virtual const ColourValue & getColorFont() const = 0;
		virtual const ColourValue & getColorOutline() const = 0;
		virtual float getLineOffset() const = 0;
		virtual float getCharOffset() const = 0;
		virtual float getMaxLength() const = 0;

		virtual ETextHorizontAlign getHorizontAlign() const = 0;
		virtual ETextVerticalAlign getVerticalAlign() const = 0;

		virtual float getCharScale() const = 0;

	public:
		virtual uint32_t getFontParams() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class VisitorTextFontInterface
	{
	public:
		virtual void onTextFont( const TextFontInterfacePtr & _font ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class TextServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("TextService")

	public:
		virtual bool loadTextEntry( const ConstString & _pakName, const FilePath & _path ) = 0;
		virtual bool unloadTextEntry( const ConstString & _pakName, const FilePath & _path ) = 0;

	public:
		virtual bool loadFonts( const ConstString & _pakName, const FilePath & _path ) = 0;
		virtual bool unloadFonts( const ConstString & _pakName, const FilePath & _path ) = 0;

	public:
		virtual bool addTextEntry( const ConstString & _key
			, const ConstString & _text
			, const ConstString & _font
			, const ColourValue & _colorFont
			, const ColourValue & _colorOutline
			, float _lineOffset
			, float _charOffset
			, float _maxLength
			, ETextHorizontAlign _horizontAlign
			, ETextVerticalAlign _verticalAlign
			, float _scale
			, uint32_t _params
			, bool _isOverride ) = 0;

	public:
		virtual bool existText( const ConstString& _key, const TextEntryInterface ** _entry ) const = 0;
		virtual const TextEntryInterface * getTextEntry( const ConstString& _key ) const = 0;

	public:
		virtual bool existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const = 0;

		virtual TextFontInterfacePtr getFont( const ConstString & _name ) const = 0;
		virtual void releaseFont( const TextFontInterfacePtr & _font ) = 0;

	public:
		virtual void visitFonts( VisitorTextFontInterface * _vistitor ) = 0;

	public:
		virtual bool directFontCompile( const ConstString & _name ) = 0;
		virtual bool directFontRelease( const ConstString & _name ) = 0;

	public:
		virtual const ConstString & getDefaultFontName() const = 0;

	public:
		virtual bool validate() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define TEXT_SERVICE( serviceProvider )\
	((Menge::TextServiceInterface*)SERVICE_GET(serviceProvider, Menge::TextServiceInterface))
}
