CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(movie)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libmovie )

add_definitions(-DAE_MOVIE_STREAM_NO_CACHE)

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/movie_math.c
${SOURCE_DIR}/src/movie_node.c
${SOURCE_DIR}/src/movie_data.c
${SOURCE_DIR}/src/movie_instance.c
${SOURCE_DIR}/src/movie_transformation.c
)

include_directories(${SOURCE_DIR}/include)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

