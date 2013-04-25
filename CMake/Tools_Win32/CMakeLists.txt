 
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
	#MESSAGE( ${ARGN} )
ENDMACRO( ADD_FILTER group_name files )


# solution

PROJECT( Tools_WIN32 )

SET(MENGINE_TARGET "WIN32")

INCLUDE( ../win32_template.cmake )

SET( MENGINE_REPOSITORY ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

SET( MENGINE_LIB_PREFIX "" )
SET( MENGINE_LIB_SUFFIX ".lib" )


# output paths
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/${MENGINE_LIB_DIR} )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/bin )

# input and thirdparty paths
SET( THIRDPARTY_DIR ${MENGINE_REPOSITORY}/dependencies )
SET( SOURCE_DIR ${MENGINE_REPOSITORY}/src )
SET( CMAKE_TEMP_DIR ${MENGINE_REPOSITORY}/CMakeTemp/WIN32_TOOLS${CMAKE_BUILD_TYPE} )

# implementation

ADD_DEFINITIONS(-D"MENGINE_CONFIG=Config_WIN32.h")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config_WIN32.h ${SOURCE_DIR}/Config/Config_WIN32.h COPYONLY)

SET( MENGE_DEPENDENCIES )
SET( APPLICATION_DEPENDENCIES )
SET( APPLICATION_LIBS )

INCLUDE_DIRECTORIES( ${SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${SOURCE_DIR}/Utils )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/boost )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Utils ${CMAKE_TEMP_DIR}/Utils )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/WindowsLayer ${CMAKE_TEMP_DIR}/WindowsLayer )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/MengeWin32FileSystem ${CMAKE_TEMP_DIR}/System/MengeWin32FileSystem )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/ICUUnicodeSystem ${CMAKE_TEMP_DIR}/System/ICUUnicodeSystem )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceProvider ${CMAKE_TEMP_DIR}/Service/ServiceProvider )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceStringize ${CMAKE_TEMP_DIR}/Service/ServiceStringize )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceArchive ${CMAKE_TEMP_DIR}/Service/ServiceArchive )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceFile ${CMAKE_TEMP_DIR}/Service/ServiceFile )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceCodec ${CMAKE_TEMP_DIR}/Service/ServiceCodec )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceConverter ${CMAKE_TEMP_DIR}/Service/ServiceConverter )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceLog ${CMAKE_TEMP_DIR}/Service/ServiceLog )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServicePlugin ${CMAKE_TEMP_DIR}/Service/ServicePlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceUnicode ${CMAKE_TEMP_DIR}/Service/ServiceUnicode )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeHotspotImageConverterPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeHotspotImageConverterPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeImageCodecPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeImageCodecPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeXmlCodecPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeXmlCodecPlugin )


ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/ToolsHotspotImageConverterPlugin ${CMAKE_TEMP_DIR}/Plugin/ToolsHotspotImageConverterPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/ToolsXmlCodecPlugin ${CMAKE_TEMP_DIR}/Plugin/ToolsXmlCodecPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/ToolsPNGAlphaSpreadingPlugin ${CMAKE_TEMP_DIR}/Plugin/ToolsPNGAlphaSpreadingPlugin )