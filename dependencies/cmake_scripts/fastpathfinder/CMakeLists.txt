CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( fastpathfinder )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../fastpathfinder )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/fastpathfinder/fastpathfinder.cpp
)

include_directories( ${SOURCE_DIR}/include )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
