set( MY_LIB_NAME FFMPEGVideoCodecPlugin )

ADD_FILTER(
src
	FFMPEGVideoCodecPlugin.def
	FFMPEGVideoCodecPlugin.h
	FFMPEGVideoCodecPlugin.cpp
    
    VideoDecoderFFMPEG.cpp
	VideoDecoderFFMPEG.h
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils )

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/ffmpeg/include )

IF(MSVC)
    INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/cmake_scripts/ffmpeg )
ENDIF(MSVC)

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX} )

IF(MSVC)
    SET(
     VIDEO_CODEC_STATIC_LIBS
     ${THIRDPARTY_DIR}/ffmpeg/bin/avcodec.lib
     ${THIRDPARTY_DIR}/ffmpeg/bin/avformat.lib
     ${THIRDPARTY_DIR}/ffmpeg/bin/avutil.lib
     ${THIRDPARTY_DIR}/ffmpeg/bin/swscale.lib
     )
ENDIF(MSVC)

IF(MINGW)
    SET(
     VIDEO_CODEC_STATIC_LIBS
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavcodec.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavformat.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavutil.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libswscale.dll.a
     )
ENDIF(MINGW)
 
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${VIDEO_CODEC_STATIC_LIBS} )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} ) 