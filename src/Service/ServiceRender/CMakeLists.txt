SET( MY_LIB_NAME ServiceRender )

ADD_FILTER(
src
    RenderEngine.h
	RenderEngine.cpp	
	RenderTexture.h
	RenderTexture.cpp
    RenderTextureManager.h
    RenderTextureManager.cpp
    RenderMaterial.h
    RenderMaterial.cpp
    RenderMaterialManager.h
    RenderMaterialManager.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/metabuf
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
