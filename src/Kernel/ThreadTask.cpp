#	include "ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTask::ThreadTask()
		: m_complete(false)
		, m_interrupt(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTask::~ThreadTask()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::main()
	{
		if( m_interrupt == true )
		{
			return false;
		}

		bool state = this->onMain();
		if( state == false )
		{
			m_interrupt = true;
			return false;
		}

		m_complete = true;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isComplete() const
	{
		return m_complete;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isInterrupt() const
	{
		return m_interrupt;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::onMain()
	{
		bool state = this->_onMain();
        if( state == false )
		{
			this->onInterrupt();
		}

		return state;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_onMain()
	{
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::onRun()
	{
		for( TVectorTaskListener::iterator
			it = m_listeners.begin(),
			it_end = m_listeners.end();
		it != it_end;
		++it )
		{
			ThreadTaskListener * listener = *it;

			listener->onTaskRun( this );
		}
    
		bool state = this->_onRun();
        return state;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::onComplete()
	{
		for( TVectorTaskListener::iterator
			it = m_listeners.begin(),
			it_end = m_listeners.end();
		it != it_end;
		++it )
		{
			ThreadTaskListener * listener = *it;

			listener->onTaskComplete( this );
		}

		this->_onComplete();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onComplete()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::cancel()
	{
		m_interrupt = true;

		this->onCancel();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::onCancel()
	{
		this->_onCancel();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onCancel()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::update()
	{
		this->onUpdate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::onUpdate()
	{
		this->_onUpdate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onUpdate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::onInterrupt()
	{
		this->_onInterrupt();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onInterrupt()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::addListener( ThreadTaskListener * _listener )
	{
		m_listeners.push_back( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	
}