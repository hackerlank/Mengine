#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "Factory/FactorablePtr.h"

#	include "stdex/thread_guard.h"

#   include <stddef.h>

namespace Menge
{
	class Factorable;

	class FactoryListenerInterface
		: public FactorablePtr
	{
	public:
		virtual void onFactoryDestroyObject( Factorable * _object ) = 0;
	};

	typedef stdex::intrusive_ptr<FactoryListenerInterface> FactoryListenerInterfacePtr;

	class Factory
		: public FactorablePtr
	{
	public:
		Factory();
		virtual ~Factory();

	public:
		void setListener( const FactoryListenerInterfacePtr & _listener );
		void setMutex( const ThreadMutexInterfacePtr & _mutex );
		
	public:
		Factorable * createObject();
		void destroyObject( Factorable * _object );

	public:
		template<class T>
		T createObjectT()
		{
			Factorable * obj = this->createObject();

#	ifdef _DEBUG
			if( dynamic_cast<T>(obj) == nullptr )
			{
                throw;
			}
#	endif

			T t = static_cast<T>(obj);

			return t;
		}

	public:
		uint32_t countObject() const;

	protected:
		virtual Factorable * _createObject() = 0;
		virtual void _destroyObject( Factorable * _object ) = 0;

	protected:
		FactoryListenerInterfacePtr m_listener;
		ThreadMutexInterfacePtr m_mutex;

		uint32_t m_count;

		STDEX_THREAD_GUARD_INIT;
	};

	typedef stdex::intrusive_ptr<Factory> FactoryPtr;
}
