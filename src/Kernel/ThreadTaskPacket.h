#	pragma once

#	include "ThreadTask.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class ThreadTaskPacket
		: public ThreadTask
	{
	public:
		ThreadTaskPacket();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, uint32_t _packetSize );

	public:
		void addTask( const ThreadTaskPtr & _task );
		const ThreadTaskPtr & getTask( uint32_t _index ) const;
		uint32_t countTask() const;
		
	protected:
		bool _onMain() override;

	protected:
		bool _onRun() override;
		void _onCancel() override;
		void _onUpdate() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef stdex::vector<ThreadTaskPtr> TVectorThreadTasks;
		TVectorThreadTasks m_tasks;
	};

	typedef stdex::intrusive_ptr<ThreadTaskPacket> ThreadTaskPacketPtr;
}