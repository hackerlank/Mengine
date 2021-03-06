 
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
	#MESSAGE( ${ARGN} )
ENDMACRO( ADD_FILTER group_name files )


# solution

PROJECT( Tools_WIN32 )

SET(MENGINE_TARGET "WIN32")

SET(MENGINE_TARGET_MSVC ON)

INCLUDE( ../win32_template.cmake )

SET( MENGINE_REPOSITORY ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

# output paths
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/${MENGINE_LIB_DIR} )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/bin_tools )

# input and thirdparty paths
SET( THIRDPARTY_DIR ${MENGINE_REPOSITORY}/dependencies )
SET( SOURCE_DIR ${MENGINE_REPOSITORY}/src )
SET( CMAKE_TEMP_DIR ${MENGINE_REPOSITORY}/CMakeTemp/Tools_WIN32/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE} )

# implementation
SET(MENGINE_ASTRALAX_PARTICLE_DLL ON)
SET(MENGINE_ASTRALAX_PARTICLE2_DLL ON)



SET( MENGE_DEBUG )
string(COMPARE EQUAL CMAKE_BUILD_TYPE Debug MENGE_DEBUG)

SET( MENGE_DEPENDENCIES )
SET( APPLICATION_DEPENDENCIES )
SET( APPLICATION_LIBS )

INCLUDE_DIRECTORIES( ${SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${SOURCE_DIR}/Utils )
INCLUDE_DIRECTORIES( ${SOURCE_DIR}/Tools )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/boost )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Utils ${CMAKE_TEMP_DIR}/Utils )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Kernel ${CMAKE_TEMP_DIR}/Kernel )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Metacode ${CMAKE_TEMP_DIR}/Metacode )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/WindowsLayer ${CMAKE_TEMP_DIR}/WindowsLayer )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/Win32UnicodeSystem ${CMAKE_TEMP_DIR}/System/Win32UnicodeSystem )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/Win32ThreadSystem ${CMAKE_TEMP_DIR}/System/Win32ThreadSystem )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/Win32PluginSystem ${CMAKE_TEMP_DIR}/System/Win32PluginSystem )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceProvider ${CMAKE_TEMP_DIR}/Service/ServiceProvider )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceStringize ${CMAKE_TEMP_DIR}/Service/ServiceStringize )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceArchive ${CMAKE_TEMP_DIR}/Service/ServiceArchive )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceFile ${CMAKE_TEMP_DIR}/Service/ServiceFile )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceCodec ${CMAKE_TEMP_DIR}/Service/ServiceCodec )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceData ${CMAKE_TEMP_DIR}/Service/ServiceData )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceMemory ${CMAKE_TEMP_DIR}/Service/ServiceMemory )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceConverter ${CMAKE_TEMP_DIR}/Service/ServiceConverter )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceLogger ${CMAKE_TEMP_DIR}/Service/ServiceLogger )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServicePlugin ${CMAKE_TEMP_DIR}/Service/ServicePlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceUnicode ${CMAKE_TEMP_DIR}/Service/ServiceUnicode )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceThread ${CMAKE_TEMP_DIR}/Service/ServiceThread )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceConfig ${CMAKE_TEMP_DIR}/Service/ServiceConfig )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Platform/Win32Platform ${CMAKE_TEMP_DIR}/Platform/Win32Platform )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeDevelopmentConverterPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeDevelopmentConverterPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeImageCodecPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeImageCodecPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeXmlCodecPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeXmlCodecPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeZipPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeZipPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeLZ4Plugin ${CMAKE_TEMP_DIR}/Plugin/MengeLZ4Plugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeWin32FileGroupPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeWin32FileGroupPlugin )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Tools ${CMAKE_TEMP_DIR}/Tools )