CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( poly2tri )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../poly2tri/poly2tri )

# OUR SOURCES
set( sources
${SOURCE_DIR}/common/shapes.cc
${SOURCE_DIR}/sweep/advancing_front.cc
${SOURCE_DIR}/sweep/cdt.cc
${SOURCE_DIR}/sweep/sweep.cc
${SOURCE_DIR}/sweep/sweep_context.cc
)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
