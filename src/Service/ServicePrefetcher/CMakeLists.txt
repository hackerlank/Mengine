SET( MY_LIB_NAME ServicePrefetcher )

ADD_FILTER(
src
    PrefetcherManager.h
	PrefetcherManager.cpp    
    ThreadTaskPrefetch.h
    ThreadTaskPrefetch.cpp
    ThreadTaskPrefetchDataflow.h
    ThreadTaskPrefetchDataflow.cpp
    ThreadTaskPrefetchImageDecoder.h
    ThreadTaskPrefetchImageDecoder.cpp
    ThreadTaskPrefetchSoundDecoder.h
    ThreadTaskPrefetchSoundDecoder.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/pybind/include
    ) 

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)
 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
