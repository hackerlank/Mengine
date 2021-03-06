cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

project(python)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Python )

REMOVE_DEFINITIONS(-Doverride)

include(ConfigureChecks.cmake)

#option(WITH_THREAD "compile in rudimentary thread support" OFF)
#option(WITH_DOC_STRINGS "Enable if you want documentation strings in extension modules" OFF)
#option(Py_USING_UNICODE "Enable unicode support" ON)
#option(Py_DEBUG "Build debugging interpreter" OFF)
#option(WITH_PYMALLOC "Define if you want to compile in Python-specific mallocs" ON)
#option(HAVE_LARGEFILE_SUPPORT "Large file support " ON)

set(extraLibs)

set(THREAD_SRC)

if(HAVE_LIBM)
  set(extraLibs ${extraLibs} ${HAVE_LIBM})
endif(HAVE_LIBM)

if(HAVE_LIBUTIL)
  set(extraLibs ${extraLibs} ${HAVE_LIBUTIL})
endif(HAVE_LIBUTIL)

set(MACHDEP_SRCS ) #conf 

set(SIGNAL_SRCS )    # configure

set(MOD_SRCS )

# this macro let's you decide whether you want to build the macro 
# and if yes, whether you want to build it statically or as module
macro(ADD_PYTHON_MODULE _NAME _STATIC_SRCS)
  option(MODULE_${_NAME}_ENABLE "Add module ${_NAME}" TRUE)

  if(MODULE_${_NAME}_ENABLE)
    option(MODULE_${_NAME}_SHARED "Add module ${_NAME} shared" FALSE)
    if(MODULE_${_NAME}_SHARED)
      message(STATUS "NOT Adding ${_NAME} as shared module (not yet implemented)")
    else(MODULE_${_NAME}_SHARED)
      message(STATUS "Adding ${_NAME} as static module")
      foreach (_current_FILE ${ARGN})
        set(${_STATIC_SRCS} ${${_STATIC_SRCS}} ${_current_FILE})
      endforeach (_current_FILE ${ARGN})
      set(PY_STATIC_MODULES_INIT_FUNCTIONS "${PY_STATIC_MODULES_INIT_FUNCTIONS}extern void init${_NAME}(void);\n")
      set(PY_STATIC_MODULES_INITTAB        "${PY_STATIC_MODULES_INITTAB}  {\"${_NAME}\", init${_NAME}},\n")
    endif(MODULE_${_NAME}_SHARED)
  endif(MODULE_${_NAME}_ENABLE)
endmacro(ADD_PYTHON_MODULE)

if(WIN32)
  set(PY_POSIX_NAME nt)
else(WIN32)
  set(PY_POSIX_NAME posix)
endif(WIN32)

#add_python_module(${PY_POSIX_NAME}  MOD_SRCS ${SOURCE_DIR}/Modules/posixmodule.c)

add_python_module(signal    MOD_SRCS ${SOURCE_DIR}/Modules/signalmodule.c)
add_python_module(errno     MOD_SRCS ${SOURCE_DIR}/Modules/errnomodule.c)
#add_python_module(pwd       MOD_SRCS ${TRUE_DIR}/Modules/pwdmodule.c)
add_python_module(_sre      MOD_SRCS ${SOURCE_DIR}/Modules/_sre.c)
add_python_module(_codecs   MOD_SRCS ${SOURCE_DIR}/Modules/_codecsmodule.c)
#add_python_module(zipimport MOD_SRCS ${SOURCE_DIR}/Modules/zipimport.c)
#add_python_module(zlib MOD_SRCS ${SOURCE_DIR}/Modules/zlibmodule.c)
add_python_module(_symtable MOD_SRCS ${SOURCE_DIR}/Modules/symtablemodule.c)
add_python_module(xxsubtype MOD_SRCS ${SOURCE_DIR}/Modules/xxsubtype.c)
add_python_module(_struct MOD_SRCS ${SOURCE_DIR}/Modules/_struct.c)
add_python_module(binascii MOD_SRCS ${SOURCE_DIR}/Modules/binascii.c)
add_python_module(unicodedata MOD_SRCS ${SOURCE_DIR}/Modules/unicodedata.c)
add_python_module(cPickle MOD_SRCS ${SOURCE_DIR}/Modules/cPickle.c)
add_python_module(cStringIO MOD_SRCS ${SOURCE_DIR}/Modules/cStringIO.c)
add_python_module(cStringIO MOD_SRCS ${SOURCE_DIR}/Modules/_weakref.c)


set( HAVE_CHROOT 0 )
set( HAVE_FSYNC 0 )
set( HAVE_WAIT3 0 )
set( HAVE_WAIT4 0 )
set( HAVE_DEVICE_MACROS 0 )
file(MAKE_DIRECTORY ${python_BINARY_DIR}/Modules)

file(WRITE ${python_BINARY_DIR}/Modules/Setup "#created by cmake, do not edit\n")
configure_file(Modified/wininit.c ${SOURCE_DIR}/Modules/wininit.c COPYONLY)

configure_file(Modified/pymem.h ${SOURCE_DIR}/Include/pymem.h COPYONLY)
configure_file(Modified/errors.c ${SOURCE_DIR}/Python/errors.c COPYONLY)

configure_file(Modified/config.c ${SOURCE_DIR}/PC/config.c COPYONLY)
configure_file(Modified/pyconfig.h ${SOURCE_DIR}/PC/pyconfig.h COPYONLY)


############################## 

set(PARSER_SRCS      ${SOURCE_DIR}/Parser/acceler.c
                ${SOURCE_DIR}/Parser/grammar1.c
                ${SOURCE_DIR}/Parser/listnode.c
                ${SOURCE_DIR}/Parser/node.c
                ${SOURCE_DIR}/Parser/parser.c
                ${SOURCE_DIR}/Parser/parsetok.c
                ${SOURCE_DIR}/Parser/bitset.c
                ${SOURCE_DIR}/Parser/metagrammar.c
                ${SOURCE_DIR}/Parser/firstsets.c
                ${SOURCE_DIR}/Parser/grammar.c
                ${SOURCE_DIR}/Parser/pgen.c
                ${SOURCE_DIR}/Parser/myreadline.c
                ${SOURCE_DIR}/Parser/tokenizer.c 
                )


set(OBJECT_SRCS ${SOURCE_DIR}/Objects/abstract.c
                ${SOURCE_DIR}/Objects/boolobject.c
                ${SOURCE_DIR}/Objects/bufferobject.c
				${SOURCE_DIR}/Objects/bytes_methods.c
				${SOURCE_DIR}/Objects/bytearrayobject.c
                ${SOURCE_DIR}/Objects/cellobject.c
                ${SOURCE_DIR}/Objects/classobject.c
                ${SOURCE_DIR}/Objects/cobject.c
                ${SOURCE_DIR}/Objects/codeobject.c
                ${SOURCE_DIR}/Objects/complexobject.c
                ${SOURCE_DIR}/Objects/descrobject.c
                ${SOURCE_DIR}/Objects/enumobject.c
                ${SOURCE_DIR}/Objects/exceptions.c
                ${SOURCE_DIR}/Objects/genobject.c
                ${SOURCE_DIR}/Objects/fileobject.c
                ${SOURCE_DIR}/Objects/floatobject.c
                ${SOURCE_DIR}/Objects/frameobject.c
                ${SOURCE_DIR}/Objects/funcobject.c
                ${SOURCE_DIR}/Objects/intobject.c
                ${SOURCE_DIR}/Objects/iterobject.c
                ${SOURCE_DIR}/Objects/listobject.c
                ${SOURCE_DIR}/Objects/longobject.c
                ${SOURCE_DIR}/Objects/dictobject.c
                ${SOURCE_DIR}/Objects/methodobject.c
                ${SOURCE_DIR}/Objects/moduleobject.c
                ${SOURCE_DIR}/Objects/object.c
                ${SOURCE_DIR}/Objects/obmalloc.c
                ${SOURCE_DIR}/Objects/rangeobject.c
                ${SOURCE_DIR}/Objects/setobject.c
                ${SOURCE_DIR}/Objects/sliceobject.c
                ${SOURCE_DIR}/Objects/stringobject.c
                ${SOURCE_DIR}/Objects/structseq.c
                ${SOURCE_DIR}/Objects/tupleobject.c
                ${SOURCE_DIR}/Objects/typeobject.c
                ${SOURCE_DIR}/Objects/weakrefobject.c
                ${SOURCE_DIR}/Objects/capsule.c
                ${SOURCE_DIR}/Objects/memoryobject.c
                
                
   )
   
#if(Py_USING_UNICODE)
   set(OBJECT_SRCS ${OBJECT_SRCS} ${SOURCE_DIR}/Objects/unicodeobject.c ${SOURCE_DIR}/Objects/unicodectype.c)
#endif(Py_USING_UNICODE)


set(PYTHON_SRCS ${SOURCE_DIR}/Python/_warnings.c
				${SOURCE_DIR}/Python/Python-ast.c 
                ${SOURCE_DIR}/Python/asdl.c 
                ${SOURCE_DIR}/Python/ast.c 
                ${SOURCE_DIR}/Python/bltinmodule.c 
                ${SOURCE_DIR}/Python/ceval.c 
                ${SOURCE_DIR}/Python/compile.c 
                ${SOURCE_DIR}/Python/codecs.c 
                ${SOURCE_DIR}/Python/errors.c 
                ${SOURCE_DIR}/Python/frozen.c 
                ${SOURCE_DIR}/Python/frozenmain.c 
                ${SOURCE_DIR}/Python/future.c 
                ${SOURCE_DIR}/Python/getargs.c 
                ${SOURCE_DIR}/Python/getcompiler.c 
                ${SOURCE_DIR}/Python/getcopyright.c 
#                ${SOURCE_DIR}/Python/getmtime.c 
                ${SOURCE_DIR}/Python/getplatform.c 
                ${SOURCE_DIR}/Python/getversion.c 
                ${SOURCE_DIR}/Python/graminit.c 
                ${SOURCE_DIR}/Python/import.c 
                ${SOURCE_DIR}/Python/importdl.c 
                ${SOURCE_DIR}/Python/marshal.c 
                ${SOURCE_DIR}/Python/modsupport.c 
                ${SOURCE_DIR}/Python/mystrtoul.c 
                ${SOURCE_DIR}/Python/mysnprintf.c 
				${SOURCE_DIR}/Python/peephole.c
                ${SOURCE_DIR}/Python/pyarena.c 
                ${SOURCE_DIR}/Python/pyfpe.c 
				${SOURCE_DIR}/Python/pymath.c
                ${SOURCE_DIR}/Python/pystate.c 
                ${SOURCE_DIR}/Python/pythonrun.c 
                ${SOURCE_DIR}/Python/structmember.c 
                ${SOURCE_DIR}/Python/symtable.c 
                ${SOURCE_DIR}/Python/sysmodule.c 
                ${SOURCE_DIR}/Python/traceback.c 
                ${SOURCE_DIR}/Python/getopt.c 
				${SOURCE_DIR}/Python/pystrcmp.c
                ${SOURCE_DIR}/Python/pystrtod.c 
				${SOURCE_DIR}/Python/formatter_unicode.c
				${SOURCE_DIR}/Python/formatter_string.c
                ${SOURCE_DIR}/Python/pyctype.c                
                ${SOURCE_DIR}/Python/dtoa.c
                ${SOURCE_DIR}/Python/random.c
                
                #${MACHDEP_SRCS}
                ${THREAD_SRC}
                ${DYNLOAD_SRC}
                )

set(MODULE_SRCS ${SOURCE_DIR}/Modules/main.c 
                ${SOURCE_DIR}/Modules/gcmodule.c
                ${SOURCE_DIR}/Modules/mathmodule.c
                #${SOURCE_DIR}/Modules/posixmodule.c
                ${SOURCE_DIR}/Modules/_math.c
                
                #${SOURCE_DIR}/Modules/timemodule.c
				#${SOURCE_DIR}/Modules/cStringIO.c
				#${SOURCE_DIR}/Modules/arraymodule.c
				#${SOURCE_DIR}/Modules/datetimemodule.c		
				
                ${MOD_SRCS} )

set( MODULE_SRCS ${MODULE_SRCS} Modified/config.c )
set( MODULE_SRCS ${MODULE_SRCS} Modified/mockup.c )                
				
if( NOT WIN32 )
	#set( MODULE_SRCS ${MODULE_SRCS} ${SOURCE_DIR}/Modules/getpath.c )    
else( NOT WIN32 )
	set( MODULE_SRCS ${MODULE_SRCS} 
	${SOURCE_DIR}/Modules/wininit.c
	${SOURCE_DIR}/PC/getpathp.c	)
endif( NOT WIN32 )

set_source_files_properties(${SOURCE_DIR}/Python/getplatform.c PROPERTIES COMPILE_FLAGS -DPLATFORM=\\"${PY_PLATFORM}\\")
set_source_files_properties(${SOURCE_DIR}/Modules/getpath.c PROPERTIES COMPILE_FLAGS  "-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DEXEC_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DVERSION=\\\"2.7.9\\\" -DVPATH=\\\"..\\\"  -DPYTHONPATH=\\\":plat-linux2:lib-tk\\\" " )

#include_directories(${SOURCE_DIR}/Modules/zlib ${SOURCE_DIR})
include_directories(${SOURCE_DIR}/Include ${SOURCE_DIR})
include_directories(${SOURCE_DIR}/Python)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../stdex/include)

include_directories(${SOURCE_DIR}/PC)

if(WIN32)  
  if(MSVC)
	  add_definitions(-DWIN32 -D_WINDOWS)
  else(MSVC)
	  add_definitions(-D_WINDOWS)
  endif(MSVC)
endif(WIN32)

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPy_BUILD_CORE)

#if(NOT HAVE_HYPOT)
 # add_definitions(-DWITHOUT_COMPLEX)
#endif(NOT HAVE_HYPOT)

# add_library(${python_LIB_NAME} ${SOURCE_DIR}/Modules/getbuildinfo.c ${SOURCE_DIR}/Modules/_typesmodule.c
              # ${PARSER_SRCS}
              # ${OBJECT_SRCS}
              # ${PYTHON_SRCS}
              # ${MODULE_SRCS}
             #${SIGNAL_SRCS}
              # )
			  
add_library( ${PROJECT_NAME}_static STATIC
			  ${SOURCE_DIR}/Modules/getbuildinfo.c 
#			  ${SOURCE_DIR}/Modules/_typesmodule.c
              ${PARSER_SRCS}
              ${OBJECT_SRCS}
              ${PYTHON_SRCS}
              ${MODULE_SRCS} )

target_link_libraries(${PROJECT_NAME}_static ${extraLibs})
#set_target_properties(pythonLib PROPERTIES OUTPUT_NAME ${python_LIB_NAME})

#add_executable(python Modules/python.c )
#target_link_libraries(python pythonLib)

#install(TARGETS pythonLib DESTINATION lib)
#install(TARGETS python DESTINATION bin)
#file(GLOB pyHeaders ${Python_SOURCE_DIR}/Include/*.h )
#install(FILES ${Python_BINARY_DIR}/pyconfig.h ${pyHeaders} DESTINATION include/python${PY_MAJOR}.${PY_MINOR})
