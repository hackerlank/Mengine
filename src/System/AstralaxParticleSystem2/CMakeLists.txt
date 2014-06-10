set( MY_LIB_NAME AstralaxParticleSystem2 )

ADD_FILTER(
src
	AstralaxEmitter2.cpp
	AstralaxEmitter2.h
	AstralaxEmitterContainer2.cpp
	AstralaxEmitterContainer2.h
	AstralaxParticleSystem2.cpp
	AstralaxParticleSystem2.h
)

SET( MAGIC_3D 0 )

if( MAGIC_3D )
    add_definitions(-DMAGIC_3D)
endif( MAGIC_3D )

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( magic_lib_path )

if( WIN32 )
 SET( magic_lib_path ${magic_lib_path}/win/lib )
elseif( APPLE )
 #if(DEFINED MENGINE_IPAD)
  SET( magic_lib_path ${magic_lib_path}/iphone )
 #endif(DEFINED MENGINE_IPAD)

 IF(${MENGINE_TARGET} STREQUAL "MACOS")
  SET( magic_lib_path ${magic_lib_path}/win/lib/mingw )
 ENDIF(${MENGINE_TARGET} STREQUAL "MACOS")
endif( WIN32 )

if( MINGW )
 SET( magic_lib_path ${magic_lib_path}/mingw )
elseif( MSVC )
 SET( magic_lib_path ${magic_lib_path}/microsoft )
endif( MINGW )

SET( magic_lib_path ${THIRDPARTY_DIR}/astralax${magic_lib_path} )
MESSAGE( ${magic_lib_path} )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

if( MAGIC_3D )
    if( MINGW )
        SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        ${THIRDPARTY_DIR}/astralax/win/lib/mingw/libmagic3d.a
        )
    elseif( WIN32 )
        SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        ${THIRDPARTY_DIR}/astralax/win/lib/microsoft/magic3d.lib
        )
    elseif( APPLE )
        IF(${MENGINE_TARGET} STREQUAL "IPAD")
            SET( APPLICATION_LIBS
            ${APPLICATION_LIBS}
            ${magic_lib_path}/libimagic.a
            )
        ENDIF(${MENGINE_TARGET} STREQUAL "IPAD")
        
        IF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
            SET( APPLICATION_LIBS
            ${APPLICATION_LIBS}
            ${magic_lib_path}/libimagic_simulator.a
            )    
        ENDIF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
    endif( MINGW )
else( MAGIC_3D )
    if( MINGW )
        SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        ${THIRDPARTY_DIR}/astralax/win/lib/mingw/libmagic.a
        )
    elseif( WIN32 )
        SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        ${THIRDPARTY_DIR}/astralax/win/lib/microsoft/magic.lib
        )
    elseif( APPLE )
        IF(${MENGINE_TARGET} STREQUAL "IPAD")
            SET( APPLICATION_LIBS
            ${APPLICATION_LIBS}
            ${magic_lib_path}/libimagic.a
            )
        ENDIF(${MENGINE_TARGET} STREQUAL "IPAD")
        
        IF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
            SET( APPLICATION_LIBS
            ${APPLICATION_LIBS}
            ${magic_lib_path}/libimagic_simulator.a
            )    
        ENDIF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
    endif( MINGW )
endif( MAGIC_3D )


SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)