#	include "ThreadEngine.h"

#   include "Interface/ConfigInterface.h"

#	include "Kernel/ThreadTask.h"
#	include "Kernel/ThreadTaskPacket.h"

#	include "Logger/Logger.h"

#	include <algorithm>

#	include "stdex/allocator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadService, Menge::ThreadEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::ThreadEngine()
		: m_threadCount(0)
		, m_avaliable(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::~ThreadEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
	{
		_mutex->lock();
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
	{
		_mutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::_initialize()
	{
		bool avaliable = CONFIG_VALUE(m_serviceProvider, "Engine", "ThreadServiceAvaliable", true );

		if( avaliable == false )
		{
			return true;
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::ThreadSystemInterface ) == false )
		{
			m_avaliable = false;

			return true;
		}

		m_avaliable = THREAD_SYSTEM(m_serviceProvider)
			->avaliable();

		if( m_avaliable == false )
		{
			return true;
		}

		m_threadCount = CONFIG_VALUE( m_serviceProvider, "Engine", "ThreadCount", 16U );

		m_allocatorPoolMutex = THREAD_SYSTEM(m_serviceProvider)
			->createMutex( "ThreadEngine::initialize" );

		stdex_allocator_initialize_threadsafe( m_allocatorPoolMutex.get()
			, (stdex_allocator_thread_lock_t)&s_stdex_thread_lock
			, (stdex_allocator_thread_unlock_t)&s_stdex_thread_unlock 
			);
                
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::_finalize()
	{	
		for( TVectorThreadTaskDesc::iterator 
			it =  m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			ThreadTaskDesc & taskThread = *it;

			const ThreadTaskInterfacePtr & task = taskThread.task;

			this->joinTask( task );
		}

		m_tasks.clear();

		for( TVectorThreads::iterator
			it = m_threads.begin(),
			it_end = m_threads.end();
		it != it_end;
		++it )
		{
			ThreadDesc & desc = *it;

			desc.identity->join();
		}

		m_threads.clear();

		stdex_allocator_finalize_threadsafe();

		m_allocatorPoolMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::avaliable() const
	{ 
		return m_avaliable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::isTaskOnProgress_( const ThreadTaskInterfacePtr & _task, ThreadIdentityPtr & _identity ) const
	{
		for( TVectorThreadTaskDesc::const_iterator 
			it = m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			const ThreadTaskDesc & desc = *it;

			const ThreadTaskInterfacePtr & task = desc.task;
			
			if( _task != task )
			{
				continue;
			}
			
			if( desc.progress == false )
			{
				return false;
			}

			_identity = desc.identity;

			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::createThread( const ConstString & _threadName, int _priority, const char * _doc )
	{
		if( m_avaliable == false )
		{
			return false;
		}

		if( this->hasThread_( _threadName ) == true )
		{
			return false;
		}

		ThreadIdentityPtr identity = THREAD_SYSTEM(m_serviceProvider)
			->createThread( _priority, _doc );

		if( identity == nullptr )
		{
			return false;
		}

		ThreadDesc td;
		td.name = _threadName;
		td.identity = identity;

		m_threads.push_back( td );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::destroyThread( const ConstString & _threadName )
	{
		for( TVectorThreads::iterator
			it = m_threads.begin(),
			it_end = m_threads.end();
		it != it_end;
		++it )
		{
			ThreadDesc & td = *it;

			if( td.name != _threadName )
			{
				continue;
			}

			td.identity->joinTask();
			td.identity->join();

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::hasThread_( const ConstString & _name ) const
	{
		for( TVectorThreads::const_iterator
			it = m_threads.begin(),
			it_end = m_threads.end();
		it != it_end;
		++it )
		{
			const ThreadDesc & td = *it;

			if( td.name == _name )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task )
	{
		if( m_avaliable == false )
		{
			return false;
		}

		if( this->hasThread_( _threadName ) == false )
		{
			return false;
		}
		
        bool state = _task->run();

        if( state == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::addTask invalid run"               
                );

            return false;
        }

        ThreadTaskDesc desc;

        desc.task = _task;
        desc.threadName = _threadName;
		desc.identity = nullptr;
		desc.progress = false;
		desc.complete = false;

        m_tasks.push_back( desc );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::joinTask( const ThreadTaskInterfacePtr & _task )
	{
		_task->cancel();

		ThreadIdentityPtr threadIdentity;
		if( this->isTaskOnProgress_( _task, threadIdentity ) == false )
		{
			return true;
		}
				
		bool successful = threadIdentity->joinTask();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadQueueInterfacePtr ThreadEngine::runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize )
	{
		if( m_avaliable == false )
		{
			return nullptr;
		}

		ThreadQueuePtr taskQueue = m_factoryThreadQueue.createObject();

		taskQueue->setServiceProvider( m_serviceProvider );
		taskQueue->setThreadName( _threadName );
		taskQueue->setThreadCount( _countThread );
		taskQueue->setPacketSize( _packetSize );

		m_threadQueues.push_back( taskQueue );

		return taskQueue;
	}
	///////////////////////////////////////////////////////////////////////////
	void ThreadEngine::update()
	{
		for( TVectorThreadTaskDesc::iterator
			it_task = m_tasks.begin(),
			it_task_end = m_tasks.end();
		it_task != it_task_end;
		++it_task )
		{
			ThreadTaskDesc & desc_task = *it_task;

			if( desc_task.complete == true )
			{
				continue;
			}

			if( desc_task.progress == true )
			{
				if( desc_task.task->isComplete() == true )
				{
					desc_task.complete = true;
					desc_task.progress = false;
				}
			}
			else
			{
				ThreadTaskInterface * task = desc_task.task.get();

				for( TVectorThreads::iterator
					it_thread = m_threads.begin(),
					it_thread_end = m_threads.end();
				it_thread != it_thread_end;
				++it_thread )
				{
					ThreadDesc & desc_thread = *it_thread;

					if( desc_thread.name != desc_task.threadName )
					{
						continue;
					}

					if( desc_thread.identity->processTask( task ) == true )
					{
						desc_task.identity = desc_thread.identity;
						desc_task.progress = true;
						break;
					}
				}
			}
		}

		for( TVectorThreadTaskDesc::size_type 
			it_task = 0,
			it_task_end = m_tasks.size();
		it_task != it_task_end;
		/*++it*/ )
		{
			const ThreadTaskDesc & handle = m_tasks[it_task];

			const ThreadTaskInterfacePtr & task = handle.task;

			if( task->update() == false )
			{
				++it_task;
			}
			else
			{
				m_tasks[it_task] = m_tasks.back();
				m_tasks.pop_back();
				--it_task_end;
			}
		}

		for( TVectorThreadQueues::size_type 
			it_task = 0,
			it_task_end = m_threadQueues.size();
		it_task != it_task_end;
		/*++it*/ )
		{
			const ThreadQueuePtr & pool = m_threadQueues[it_task];

			if( pool->update() == false )
			{
				++it_task;
			}
			else
			{
				m_threadQueues[it_task] = m_threadQueues.back();
				m_tasks.pop_back();
				--it_task_end;
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr ThreadEngine::createMutex( const char * _doc )
    {
		if( m_avaliable == false )
		{
			 ThreadMutexInterfacePtr mutex_dummy = 
				 m_poolThreadMutexDummy.createObject();

			return mutex_dummy;
		}

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM(m_serviceProvider)
            ->createMutex( _doc );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadEngine::sleep( unsigned int _ms )
    {
        THREAD_SYSTEM(m_serviceProvider)
            ->sleep( _ms );
    }
	//////////////////////////////////////////////////////////////////////////
	ptrdiff_t ThreadEngine::getCurrentThreadId()
	{
		if( m_avaliable == false )
		{
			return 0U;
		}

		ptrdiff_t id = THREAD_SYSTEM(m_serviceProvider)
			->getCurrentThreadId();

		return id;
	}
}	// namespace Menge
