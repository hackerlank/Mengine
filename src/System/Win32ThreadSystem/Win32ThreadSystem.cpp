#	include "Win32ThreadSystem.h"
#	include "Win32ThreadIdentity.h"

#	include "Logger/Logger.h"

#   include <algorithm>

SERVICE_FACTORY( ThreadSystem, Menge::ThreadSystemInterface, Menge::Win32ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::Win32ThreadSystem()
		: m_serviceProvider(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::~Win32ThreadSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Win32ThreadSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::initialize()		
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::finalize()
	{
		for(TVectorPosixThreadIdentity::iterator
			it = m_threadIdentities.begin(),
			it_end = m_threadIdentities.end();
		it != it_end;
		++it )
		{
			Win32ThreadIdentity * identity = *it;

            HANDLE handle = identity->getHandle();
            CloseHandle( handle );

			delete identity;
		}

		m_threadIdentities.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    static DWORD WINAPI s_tread_job( LPVOID _userData )
    {
        //ThreadHolder * threadHolder = (ThreadHolder *)_threadHolder;
        ThreadListener * threadListener = static_cast<ThreadListener*>(_userData);

        threadListener->main();

        return 0;
    }
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * Win32ThreadSystem::createThread( ThreadListener * _thread )
	{
        HANDLE handle = CreateThread( NULL, 0, &s_tread_job, (LPVOID)_thread, 0, NULL);

        if( handle == NULL )
        {
            DWORD error_code = GetLastError();

            LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread: invalid create thread error code - %d"
                , error_code
                );

            return NULL;
        }

        Win32ThreadIdentity * identity = new Win32ThreadIdentity(handle);
        m_threadIdentities.push_back( identity );
		
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::joinThread( ThreadIdentity * _thread )
	{
		Win32ThreadIdentity * identity = static_cast<Win32ThreadIdentity*>(_thread);

		HANDLE treadHandle = identity->getHandle();

		WaitForSingleObject( treadHandle, INFINITE );

        CloseHandle( treadHandle );

        m_threadIdentities.erase( 
            std::remove( m_threadIdentities.begin(), m_threadIdentities.end(), identity )
            , m_threadIdentities.end() 
            );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::sleep( unsigned int _ms )
	{
		Sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
}