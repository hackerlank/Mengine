@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Tools_Win32" "..\vs12_solution_tools_relwithinfo" "Visual Studio 12 2013" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='RelWithDebInfo'" "-DMENGINE_LIB_DIR:STRING='build_msvc12_relwithinfo'" "-DCMAKE_BUILD_TYPE:STRING='RelWithDebInfo'"
@popd

@pause
@echo on
