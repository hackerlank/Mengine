#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Utils/Factory/Factorable.h"

#	include "Factory/FactorablePtr.h"

#	include "Core/ConstString.h"

namespace Menge
{
    class ThreadTaskInterface
        : public FactorablePtr
    {
	public:
		virtual bool isRun() const = 0;
		virtual bool isComplete() const = 0;
		virtual bool isSuccessful() const = 0;
		virtual bool isCancel() const = 0;

    public:
		virtual void main() = 0;

	public:
		virtual bool run() = 0;
        virtual bool cancel() = 0;
        virtual bool update() = 0;
    };

	typedef stdex::intrusive_ptr<ThreadTaskInterface> ThreadTaskInterfacePtr;

	class ThreadQueueInterface
		: public FactorablePtr
	{
	public:
		virtual void addTask( const ThreadTaskInterfacePtr & _task ) = 0;

	public:
		virtual void cancel() = 0;
	};

	typedef stdex::intrusive_ptr<ThreadQueueInterface> ThreadQueueInterfacePtr;

	class ThreadIdentity
		: public FactorablePtr
	{
	public:
		virtual bool processTask( ThreadTaskInterface * _task ) = 0;
		virtual bool joinTask() = 0;

	public:
		virtual void join() = 0;
	};

	typedef stdex::intrusive_ptr<ThreadIdentity> ThreadIdentityPtr;

    class ThreadMutexInterface
        : public FactorablePtr
    {
    public:
        virtual void lock() = 0;
        virtual void unlock() = 0;

	public:
		virtual bool try_lock() = 0;
    };

	typedef stdex::intrusive_ptr<ThreadMutexInterface> ThreadMutexInterfacePtr;

	class ThreadSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ThreadSystem")

	public:
		virtual bool avaliable() const = 0;

	public:
		virtual ThreadIdentityPtr createThread( int _priority, const char * _doc ) = 0;

	public:
		virtual void sleep( uint32_t _ms ) = 0;

	public:
		virtual ThreadMutexInterfacePtr createMutex( const char * _doc ) = 0;

	public:
		virtual ptrdiff_t getCurrentThreadId() const = 0;
	};

#   define THREAD_SYSTEM( serviceProvider )\
    ((Menge::ThreadSystemInterface*)SERVICE_GET(serviceProvider, Menge::ThreadSystemInterface))

    class ThreadServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ThreadService")

	public:
		virtual bool avaliable() const = 0;

    public:
        virtual void update() = 0;

	public:
		virtual bool createThread( const ConstString & _threadName, int _priority, const char * _doc ) = 0;
		virtual bool destroyThread( const ConstString & _threadName ) = 0;

    public:
        virtual bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) = 0;
        virtual bool joinTask( const ThreadTaskInterfacePtr & _task ) = 0;

	public:
		virtual ThreadQueueInterfacePtr runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize ) = 0;

    public:
		virtual ThreadMutexInterfacePtr createMutex( const char * _doc ) = 0;

	public:
		virtual void sleep( unsigned int _ms ) = 0;

	public:
		virtual ptrdiff_t getCurrentThreadId() = 0;
    };

#   define THREAD_SERVICE( serviceProvider )\
    ((Menge::ThreadServiceInterface*)SERVICE_GET(serviceProvider, Menge::ThreadServiceInterface))
}