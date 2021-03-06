#	include "ResourceParticle.h"

#	include "Metacode/Metacode.h"

#	include "ResourceImageDefault.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceParticle::ResourceParticle()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceParticle::setFilePath( const FilePath& _path )
	{
		m_fileName = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceParticle::getFilePath() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_DataBlock::Meta_ResourceParticle * metadata
			= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceParticle *>(_meta);

		metadata->swap_File_Path( m_fileName );
		metadata->swap_File_Converter( m_converterType );

		uint32_t atlasCount = metadata->get_AtlasCount_Value();

		m_resourceImageNames.resize( atlasCount );

		const Metacode::Meta_DataBlock::Meta_ResourceParticle::TVectorMeta_Atlas & includes_atlas = metadata->get_IncludesAtlas();

		for( Metacode::Meta_DataBlock::Meta_ResourceParticle::TVectorMeta_Atlas::const_iterator
			it = includes_atlas.begin(),
			it_end = includes_atlas.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & atlas = *it;

			uint32_t index = atlas.get_Index();
			const Menge::ConstString & resourceName = atlas.get_ResourceName();

			m_resourceImageNames[index] = resourceName;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_convert()
	{
		bool result = this->convertDefault2_( m_converterType, m_fileName, m_fileName );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_isValid() const
	{
		if( PARTICLE_SERVICE2( m_serviceProvider )->isAvailable() == false )
		{
			return true;
		}

		const ConstString & category = this->getCategory();

		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2(m_serviceProvider)
			->createEmitterContainerFromFile( category, m_fileName );
		
		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceParticle::_isValid %s can't create container file '%s'"
				, m_name.c_str()
				, m_fileName.c_str() 
				);

			return false;
		}

		if( container->isValid() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceParticle::_isValid %s can't valid container '%s'"
				, m_name.c_str()
				, m_fileName.c_str() 
				);

			return false;
		}

		for( TVectorResourceImageName::const_iterator
			it = m_resourceImageNames.begin(),
			it_end = m_resourceImageNames.end();
		it != it_end;
		++it )
		{            
			const ConstString & name = *it;

			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResource( name, nullptr ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceParticle::_isValid %s can't get image resource '%s'"
					, m_name.c_str()
					, name.c_str()
					);

				return false;
			}            
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_compile()
	{
		const ConstString & category = this->getCategory();

		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2( m_serviceProvider )
			->createEmitterContainerFromFile( category, m_fileName );

		if( container == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ResourceParticle::_compile %s can't create container file '%s'"
				, m_name.c_str()
				, m_fileName.c_str()
				);

			return false;
		}

		for( TVectorResourceImageName::size_type
			it = 0,
			it_end = m_resourceImageNames.size();
		it != it_end;
		++it )
		{
			const ConstString & resourceName = m_resourceImageNames[it];

			ResourceImagePtr resourceImage = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceImagePtr>( resourceName );

			if( resourceImage == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceParticle::_compile %s container %s can't get atlas image %s"
					, this->getName().c_str()
					, m_fileName.c_str()
					, resourceName.c_str()
					);

				return false;
			}

			container->setAtlasResourceImage( it, resourceImage );
		}

		m_container = container;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceParticle::_release()
	{
		m_container = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterInterfacePtr ResourceParticle::createEmitter()
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("ResourceParticle::createEmitter %s not compile"
				, this->getName().c_str()
				);

			return nullptr;
		}

		ParticleEmitterInterfacePtr emitter = m_container->createEmitter();

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
}
