set( MY_LIB_NAME MoviePlugin )

ADD_FILTER(
src
	MoviePlugin.h
	MoviePlugin.cpp   
    MoviePlugin.def
    
    Movie2.cpp
    Movie2.h
    ResourceMovie2.cpp
    ResourceMovie2.h
)

add_definitions(-DAE_MOVIE_STREAM_NO_CACHE)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
        ${THIRDPARTY_DIR}/libmovie/include
        ${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/metabuf
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Menge
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Logger
    Core
    Codec
    Archive
    Factory
    Kernel
    Menge
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}movie_static${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)