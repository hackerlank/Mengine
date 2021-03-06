#   include "ProfilerService.h"

//#   include "Interface/ApplicationInterface.h"

////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ProfilerService, Menge::ProfilerService);
////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ProfilerService::ProfilerService()
        : m_checkpoint(nullptr)
        , m_beginMemoryUsage(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ProfilerService::~ProfilerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ProfilerService::_initialize()
    {
        //m_beginMemoryUsage = PLATFORM_SERVICE(m_serviceProvider)
        //    ->getMemoryUsage();
        
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void ProfilerService::_finalize()
	{
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
    void ProfilerService::memoryBegin()
    {
        //m_checkpoint = PLATFORM_SERVICE(m_serviceProvider)
        //    ->checkpointMemory();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ProfilerService::memoryEnd()
    {
        //size_t memory = PLATFORM_SERVICE(m_serviceProvider)
        //    ->diffMemory( m_checkpoint );

        m_checkpoint = nullptr;

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ProfilerService::getMemoryUsage() const
    {
        //size_t memory = PLATFORM_SERVICE(m_serviceProvider)
        //    ->getMemoryUsage();

        //size_t memory_pr = memory - m_beginMemoryUsage;
        //float mb = float(memory_pr) / (1024.f * 1024.f);

        return 0;
    }
}