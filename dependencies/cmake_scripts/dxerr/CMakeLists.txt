CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( dxerr )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../dxerr )

# OUR SOURCES
set( sources
${SOURCE_DIR}/dxerr15.cpp
)

include_directories( ${SOURCE_DIR}/include )

#INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../../DirectX/dx9/include )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
