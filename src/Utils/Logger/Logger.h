#	pragma once

#	include "Interface/LoggerInterface.h"

namespace Menge
{	
	class LoggerOperator
	{
	public:
		LoggerOperator( LoggerServiceInterface * _logger, EMessageLevel _level, uint32_t _flag );

	public:
		LoggerOperator & operator()( const char * _format, ... );

	public:
		void logMessage( const char * _format, size_t _size );

	protected:
		LoggerServiceInterface * m_log;
		EMessageLevel m_level;
        uint32_t m_flag;
	};
}

#	define LOGGER_VERBOSE_LEVEL( LOGGER, LEVEL )\
	if( LOGGER->validMessage(LEVEL, 0) == false) {} else Menge::LoggerOperator(LOGGER, LEVEL, 0)

#	define LOGGER_FATAL( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(serviceProvider), Menge::LM_FATAL )

#	define LOGGER_CRITICAL( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(serviceProvider), Menge::LM_CRITICAL )

#	define LOGGER_ERROR( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(serviceProvider), Menge::LM_ERROR )

#	define LOGGER_PERFORMANCE( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(serviceProvider), Menge::LM_PERFOMANCE )
		
#	define LOGGER_WARNING( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(serviceProvider), Menge::LM_WARNING )
	
#	define LOGGER_INFO( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(serviceProvider), Menge::LM_INFO )

