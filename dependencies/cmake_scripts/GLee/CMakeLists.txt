
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( GLee )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../GLee )

# OUR SOURCES
set( GLee_sources
	${SOURCE_DIR}/GLee.h
	${SOURCE_DIR}/GLee.c
)

include_directories(${SOURCE_DIR})
add_library( ${PROJECT_NAME}_static STATIC ${GLee_sources} )
