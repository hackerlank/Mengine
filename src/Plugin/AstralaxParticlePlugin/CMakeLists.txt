set( MY_LIB_NAME AstralaxParticlePlugin2 )

ADD_FILTER(
src
	AstralaxEmitter2.cpp
	AstralaxEmitter2.h
	AstralaxEmitterContainer2.cpp
	AstralaxEmitterContainer2.h
	AstralaxParticleSystem2.cpp
	AstralaxParticleSystem2.h
    
    AstralaxParticlePlugin2.cpp
    AstralaxParticlePlugin2.h
    AstralaxParticlePlugin2.def
)

if( DEFINED MENGINE_ASTRALAX_PARTICLE2_DLL )
    add_definitions(-DMENGE_PLUGIN_DLL)
endif()

add_definitions(-DMAGIC_3D)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/astralax 
    ${THIRDPARTY_DIR}/pybind/include
    )

if( DEFINED MENGINE_ASTRALAX_PARTICLE2_DLL )
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
else()
    ADD_LIBRARY( ${MY_LIB_NAME} STATIC ${SRC_FILES} )
endif()    

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX} )
#TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}astralax_static${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/libmagic3d.a)
elseif( WIN32 )
    if( DEFINED MENGINE_PLATFORM_WIN32 )
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.lib)
    endif()
    
    if( DEFINED MENGINE_PLATFORM_WIN64 )
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d_x64.lib)
    endif()
endif( MINGW )

if( DEFINED MENGINE_ASTRALAX_PARTICLE2_DLL )    
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

    foreach(dependency ${APPLICATION_DEPENDENCIES})
        #MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
        TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
        TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
    endforeach(dependency ${APPLICATION_DEPENDENCIES})

    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} )     
else()   
    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     ${MY_LIB_NAME}
    )

    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     PARENT_SCOPE
    )    
endif()

