#	pragma once

#   include "Interface/PackageInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#	include "Core/String.h"
#   include "Core/FilePath.h"

namespace Menge
{
	class Package
		: public PackageInterface
	{
	public:
		Package();

	public:
		void setup( ServiceProviderInterface * _serviceProvider
			, const ConstString & _name
			, const ConstString & _type
			, const ConstString & _locale
			, const ConstString & _platform
			, const ConstString & _descriptionPath
			, const ConstString & _path
			, bool _preload
			);

	public:
		const ConstString & getName() const override;

	public:
		void setPreload( bool _value ) override;
		bool isPreload() const override;

		void setLocale( const ConstString & _locale ) override;
		const ConstString & getLocale() const override;

		void setPlatfrom( const ConstString & _platform ) override;
		const ConstString & getPlatfrom() const override;

		void setPath( const ConstString & _path ) override;
		const ConstString & getPath() const override;

	public:
		bool load() override;
		bool enable() override;
		bool disable() override;

	public:
		bool validate() override;

	public:
		bool isLoad() const override;
		bool isEnable() const override;
		
	protected:
		bool mountFileGroup_();
		bool loadPak_();

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const FilePath & _path, bool _ignored );
		void addTextPath_( const FilePath & _path );
		void addScriptPak_( const FilePath & _path, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer );
		void addFontPath_( const ConstString & _font );
		void addData_( const ConstString & _name, const FilePath & _path );
		void addMaterial_( const ConstString & _path );

    protected:
		bool loadText_( const ConstString & _pakName, const FilePath & _path );
		bool unloadText_( const ConstString & _pakName, const FilePath & _path );
		bool loadFont_( const ConstString & _pakName, const FilePath & _path );
		bool unloadFont_( const ConstString & _pakName, const FilePath & _path );
		bool addUserData_( const ConstString & _pakName, const ConstString & _name, const FilePath & _path );
		bool removeUserData_( const ConstString & _name );
		bool loadMaterials_( const ConstString & _pakName, const FilePath & _path );
		bool unloadMaterials_( const ConstString & _pakName, const FilePath & _path );
				
	protected:
        ServiceProviderInterface * m_serviceProvider;

		struct PakResourceDesc
		{			
			FilePath path;
			bool ignored;
		};

		typedef stdex::vector<PakResourceDesc> TVectorPakResourceDesc;
		TVectorPakResourceDesc m_resourcesDesc;
				
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		ConstString m_platform;		
		FilePath m_descriptionPath;

		ConstString m_path;

		TVectorScriptModulePack m_scriptsPackages;

		TVectorFilePath m_pathFonts;
		TVectorFilePath m_pathTexts;

		struct PakDataDesc
		{
			ConstString name;
			FilePath path;
		};

		typedef stdex::vector<PakDataDesc> TVectorPakDataDesc;
		TVectorPakDataDesc m_datas;

		TVectorFilePath m_pathMaterials;

        bool m_preload;
		bool m_load;
		bool m_enable;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Package> PackagePtr;
}