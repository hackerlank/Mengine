#   pragma once

#   include "Interface/PluginInterface.h"

namespace Menge
{
    class Win32PluginSystem
		: public ServiceBase<PluginSystemInterface>
    {
    public:
		Win32PluginSystem();
		~Win32PluginSystem();

    public:
		DynamicLibraryInterface * loadDynamicLibrary( const WString & _dynamicLibraryName ) override;
	};
}