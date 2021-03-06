#	include "ScriptEngine.h"

#   include "ScriptModuleLoader.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/PrototypeManagerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "pybind/debug.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    extern int Py_ErrFormatFlag;
#ifdef __cplusplus
}
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Menge::ScriptEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class My_observer_bind_call
			: public pybind::observer_bind_call
		{
		public:
			My_observer_bind_call( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
			}

		public:
			void begin_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
			{
				(void)_kwds;
				(void)_args;
				(void)_functionName;
				(void)_className;

				LOGGER_INFO( m_serviceProvider )("pybind call begin %s %s"
					, _className
					, _functionName
					);

				size_t count = LOGGER_SERVICE( m_serviceProvider )
					->getCountMessage( LM_ERROR );

				m_counts.push_back( count );
			}

			void end_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
			{
				(void)_kwds;
				(void)_args;

				LOGGER_INFO( m_serviceProvider )("pybind call end %s %s"
					, _className
					, _functionName
					);

				size_t count = LOGGER_SERVICE( m_serviceProvider )
					->getCountMessage( LM_ERROR );

				size_t last_count = m_counts.back();
				m_counts.pop_back();

				if( last_count == count )
				{
					return;
				}

				if( strcmp( _className, "ScriptLogger" ) == 0 )
				{
					return;
				}

				if( strcmp( _className, "ErrorScriptLogger" ) == 0 )
				{
					return;
				}

				LOGGER_ERROR( m_serviceProvider )("script call %s::%s and get error!"
					, _className
					, _functionName
					);

				bool exist;
				PyObject * module_traceback = pybind::module_import( "traceback", exist );

				if( module_traceback == nullptr )
				{
					return;
				}

				pybind::call_method( module_traceback, "print_stack", "()" );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;

			typedef std::vector<size_t> TVectorStackMsgCount;
			TVectorStackMsgCount m_counts;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_moduleFinder(nullptr)
        , m_moduleMenge(nullptr)
		, m_loggerWarning(nullptr)
        , m_loggerError(nullptr)
		, m_initializeModules(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_pybind_logger( ServiceProviderInterface * _serviceProvider, const char * _msg )
	{
		LOGGER_ERROR( _serviceProvider )("%s"
			, _msg
			);
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::_initialize()
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );

		if( developmentMode == true )
        {
            Py_ErrFormatFlag = 1;
        }
        else
        {
            Py_ErrFormatFlag = 0;
        }

		if( pybind::initialize( false, false, true ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initialize invalid initialize pybind"
				);

			return false;
		}

		pybind::set_logger( (pybind::pybind_logger_t)s_pybind_logger, m_serviceProvider);

#   if defined(WIN32) && defined(_DEBUG)
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#	endif

		m_moduleMenge = this->initModule( "Menge" );

        this->addGlobalModule( "Menge"
			, (ScriptObject *)m_moduleMenge
			);

        uint32_t python_version = pybind::get_python_version();

		this->addGlobalModule( "_PYTHON_VERSION"
			, (ScriptObject *)pybind::ptr( python_version )
			);
		
		pybind::set_currentmodule( m_moduleMenge );

		pybind::interface_<ScriptLogger>( "ScriptLogger", true )
			.def_native( "write", &ScriptLogger::py_write )
			.def_property( "softspace", &ScriptLogger::getSoftspace, &ScriptLogger::setSoftspace )
			;

		m_loggerWarning = new ScriptLogger(m_serviceProvider);

		m_loggerWarning->setMessageLevel( LM_WARNING );

		PyObject * py_logger = pybind::ptr( m_loggerWarning );
		pybind::setStdOutHandle( py_logger );
		pybind::decref( py_logger );

		m_loggerError = new ScriptLogger(m_serviceProvider);

		m_loggerError->setMessageLevel( LM_ERROR );

		PyObject * py_loggerError = pybind::ptr( m_loggerError );
		pybind::setStdErrorHandle( py_loggerError );
		pybind::decref( py_loggerError );

		pybind::set_observer_bind_call( new My_observer_bind_call( m_serviceProvider ) );

        pybind::interface_<ScriptModuleFinder>("ScriptModuleFinder", true)
            .def("find_module", &ScriptModuleFinder::find_module)   
			.def("load_module", &ScriptModuleFinder::load_module)
            ;
        
        m_moduleFinder = new ScriptModuleFinder();

		if( m_moduleFinder->initialize( m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initialize invalid initialize ScriptModuleFinder"
				);

			return false;
		}
       
        PyObject * py_moduleFinder = pybind::ptr( m_moduleFinder );

		m_moduleFinder->setEmbed( py_moduleFinder );

		//pybind::decref( m_moduleMenge );

        pybind::_set_module_finder( py_moduleFinder );
				
		//bool gc_exist;
		//PyObject * gc = pybind::module_import( "gc", gc_exist );

		//pybind::call_method( gc, "disable", "()" );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::_finalize()
	{
		pybind::_remove_module_finder();

        this->removeGlobalModule( "Menge" );
		this->removeGlobalModule( "_PYTHON_VERSION" );

		for( TMapScriptWrapper::iterator 
			it = m_scriptWrapper.begin(),
			it_end = m_scriptWrapper.end();
		it != it_end;
		++it )
		{
			ScriptWrapperInterface * scriptClass = it->second;
			
            scriptClass->destroy();
		}

		//pybind::decref( m_moduleMenge );

		m_scriptWrapper.clear();       

		if( m_moduleFinder != nullptr )
		{
			m_moduleFinder->finalize();

			delete m_moduleFinder;
			m_moduleFinder = nullptr;
		}

		delete m_loggerWarning;
		m_loggerWarning = nullptr;

        pybind::setStdOutHandle( nullptr );

		delete m_loggerError;
		m_loggerError = nullptr;

        pybind::setStdErrorHandle( nullptr );

		pybind::finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addModulePath( const ConstString & _pak, const TVectorScriptModulePack & _modules )
	{
		if( _modules.empty() == true )
		{
			return;
		}

		TVectorConstString pathes;
		
		for( TVectorScriptModulePack::const_iterator
			it = _modules.begin(),
			it_end = _modules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

			pathes.push_back( pak.path );
		}

        m_moduleFinder->addModulePath( _pak, pathes );

		m_bootstrapperModules.insert( m_bootstrapperModules.end(), _modules.begin(), _modules.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FBootstrapperModuleRemove
		{
		public:
			FBootstrapperModuleRemove( const ScriptModulePack & _pack )
				: m_pack( _pack )
			{
			}

		protected:
			void operator = (const FBootstrapperModuleRemove &)
			{
			}

		public:
			bool operator() ( const ScriptModulePack & _pack )
			{
				if( _pack.module < m_pack.module )
				{
					return false;
				}

				if( _pack.path < m_pack.path )
				{
					return false;
				}

				return true;
			}

		protected:
			const ScriptModulePack & m_pack;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules )
	{
		m_moduleFinder->removeModulePath( _pack );

		for( TVectorScriptModulePack::const_iterator
			it = _modules.begin(),
			it_end = _modules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pack = *it;

			m_bootstrapperModules.erase( 
				std::remove_if( m_bootstrapperModules.begin(), m_bootstrapperModules.end(), FBootstrapperModuleRemove( pack ) )
				, m_bootstrapperModules.end() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::bootstrapModules()
	{
		for( TVectorScriptModulePack::const_iterator
			it = m_bootstrapperModules.begin(),
			it_end = m_bootstrapperModules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

			if( pak.module.empty() == true )
			{
				continue;
			}

			ScriptModuleInterfacePtr module = this->importModule( pak.module );

			if( module == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::bootstrapModules invalid import module %s"
					, pak.module.c_str()
					);

				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::initializeModules()
	{
		if( m_initializeModules == true )
		{
			return false;
		}

		for( TVectorScriptModulePack::const_iterator
			it = m_bootstrapperModules.begin(),
			it_end = m_bootstrapperModules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

			if( pak.module.empty() == true )
			{
				continue;
			}

			ScriptModuleInterfacePtr module = this->importModule( pak.module );

			if( module == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules invalid import module %s"
					, pak.module.c_str()
					);

				return false;
			}

			if( pak.initializer.empty() == true )
			{
				continue;
			}

			if( module->onInitialize( pak.initializer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules invalid initialize module %s"
					, pak.module.c_str()
					);

				return false;
			}
		}

		m_initializeModules = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::finalizeModules()
	{
		if( m_initializeModules == false )
		{
			return true;
		}

		m_initializeModules = false;

		for( TVectorScriptModulePack::const_reverse_iterator
			it = m_bootstrapperModules.rbegin(),
			it_end = m_bootstrapperModules.rend();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

			if( pak.module.empty() == true )
			{
				continue;
			}

			if( pak.finalizer.empty() == true )
			{
				continue;
			}

			ScriptModuleInterfacePtr module = this->importModule( pak.module );

			if( module == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("ScriptEngine::finalizeModules invalid import module %s"
					, pak.module.c_str()
					);

				return false;
			}

			if( module->onFinalize( pak.finalizer ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ScriptEngine::finalizeModules module '%s' invalid call finalizer"
					, pak.module.c_str()
					);

				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const char * _name )
	{
		LOGGER_INFO(m_serviceProvider)( "init module '%s'"
			, _name 
			);

		try
		{
			PyObject * module = pybind::module_init( _name );

			return module;
		}
		catch( ... )
		{
			ScriptEngine::handleException();
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptModuleInterfacePtr ScriptEngine::importModule( const ConstString & _name )
	{
		PyObject * py_module = nullptr;
		bool exist = false;

		try
		{
			py_module = pybind::module_import( _name.c_str(), exist );
		}
		catch( ... )
		{
			ScriptEngine::handleException();

			LOGGER_ERROR(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(c-exception)"
				, _name.c_str()
				);

			return nullptr;
		}

		if( exist == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(not exist)"
				, _name.c_str()
				);

			return nullptr;
		}

		if( py_module == nullptr )
		{			
			LOGGER_ERROR(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(script)"
				, _name.c_str()
				);

			return nullptr;
		}

		ScriptModulePtr module = m_factoryScriptModule.createObject();

		if( module->initialize( pybind::object(py_module) ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("ScriptEngine: invalid import initialize '%s'(script)"
				, _name.c_str()
				);

			return nullptr;
		}

		return module;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setCurrentModule( ScriptObject * _module )
	{
		pybind::set_currentmodule( (PyObject*)_module );
	}
    //////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addGlobalModule( const Char * _name, ScriptObject * _module )
    {        
        PyObject * builtins = pybind::get_builtins();

        PyObject * dir_bltin = pybind::module_dict( builtins );

        pybind::dict_set_t( dir_bltin, _name, (PyObject*)_module );
    }
    //////////////////////////////////////////////////////////////////////////
	void ScriptEngine::removeGlobalModule( const Char * _name )
    {
        PyObject * builtins = pybind::get_builtins();

        PyObject * dir_bltin = pybind::module_dict( builtins );

        pybind::dict_remove_t( dir_bltin, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::stringize( ScriptObject * _object, ConstString & _cstr )
    {
        if( pybind::string_check( (PyObject*)_object ) == false )
        {            
            LOGGER_ERROR(m_serviceProvider)("ScriptEngine::stringize invalid stringize object %s"
                , pybind::object_repr( (PyObject*)_object )
                );

            return false;
        }

        ConstStringHolderPythonString * holder = m_factoryPythonString.createObject();

        holder->setPythonObject( (PyObject*)_object );

        if( STRINGIZE_SERVICE( m_serviceProvider )
            ->stringizeExternal( holder, _cstr ) == false )
        {
            holder->destroy();
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setWrapper( const ConstString & _type, ScriptWrapperInterface * _wrapper )
	{
		_wrapper->setServiceProvider( m_serviceProvider );

		if( _wrapper->initialize() == false )
		{
			return;
		}

		m_scriptWrapper.insert( std::make_pair(_type, _wrapper) );
	}
	//////////////////////////////////////////////////////////////////////////|
	ScriptWrapperInterface * ScriptEngine::getWrapper( const ConstString & _type ) const
	{
		TMapScriptWrapper::const_iterator it_found = m_scriptWrapper.find( _type );

		if( it_found == m_scriptWrapper.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("ScriptEngine::wrap not found type %s"
                , _type.c_str()
                );

			return nullptr;
		}

		ScriptWrapperInterface * wrapper = it_found->second;

		return wrapper;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}
	//////////////////////////////////////////////////////////////////////////
}
