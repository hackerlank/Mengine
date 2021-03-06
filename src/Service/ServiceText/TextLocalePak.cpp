#	include "TextLocalePak.h"

#   include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include <stdex/xml_sax_parser.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLocalePack::TextLocalePack()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextLocalePack::~TextLocalePack()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextLocalePack::initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _pakName, const FilePath & _path )
	{
		m_serviceProvider = _serviceProvider;
		m_pakName = _pakName;
		m_path = _path;

		InputStreamInterfacePtr xml_text = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pakName, _path, false );

		if( xml_text == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource invalid open file %s:%s"
				, _pakName.c_str()
				, _path.c_str()
				);

			return false;
		}

		size_t xml_buffer_size = xml_text->size();

		m_memory = MEMORY_SERVICE( m_serviceProvider )
			->createMemory();

		Char * memory_buffer = m_memory->newMemory( xml_buffer_size + 1 );

		xml_text->read( memory_buffer, xml_buffer_size );
		memory_buffer[xml_buffer_size] = '\0';

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextLocalePack::getPackName() const
	{
		return m_pakName;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & TextLocalePack::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInterfacePtr TextLocalePack::getXmlBuffer() const
	{
		return m_memory;
	}
}