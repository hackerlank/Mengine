#	pragma once

#   include "Interface/AccountInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

#	include "pybind/object.hpp"

namespace Menge
{
	class Account
        : public AccountInterface
	{		
	public:
		Account();
		~Account();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const WString & _name, const ConstString & _folder, uint32_t _projectVersion );
        
	public:
		const WString & getName() const override;
        const ConstString & getFolder() const override;

    public:
		bool addSetting( const ConstString & _setting, const WString& _defaultValue, const pybind::object & _applyFunc ) override;
		bool changeSetting( const ConstString & _setting, const WString& _value ) override;
		const WString& getSetting( const ConstString & _setting ) const override;
        bool hasSetting( const ConstString & _setting ) const override;

	public:
		void apply() override;

		bool load() override;
		bool save() override;

	public:
		InputStreamInterfacePtr openReadBinaryFile( const FilePath & _fileName ) override;
		OutputStreamInterfacePtr openWriteBinaryFile( const FilePath & _fileName ) override;

    public:
		MemoryCacheBufferInterfacePtr loadBinaryFile( const ConstString & _fileName ) override;
        bool writeBinaryFile( const ConstString & _fileName, const void * _data, size_t _size ) override;
		bool hasBinaryFile( const ConstString & _filename ) const override;
	
	protected:
        ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

		WString m_name;
		uint32_t m_projectVersion;

        ConstString m_folder;
        ConstString m_settingsPath;
        		
        struct Setting
        {
            WString value;
            pybind::object cb;
        };

		typedef stdex::map<ConstString, Setting> TMapSettings;
		TMapSettings m_settings;
	};

	typedef stdex::intrusive_ptr<Account> AccountPtr;
}	// namespace Menge
