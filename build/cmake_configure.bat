
@set cmake_tool=%1
@if "%cmake_tool%"=="" goto error

@set source_dir=%2
@if "%source_dir%"=="" goto error

@set build_dir=%3
@if "%build_dir%"=="" set build_dir="build"

@set generator=%4
@if %generator%=="" set generator="Unix Makefiles"

@set configuration=%5
@if "%configuration%"=="" set configuration="Release"

@set cmake_params=%6
@set cmake_params2=%7

@if %generator%=="NMake Makefiles" call "%VS80COMNTOOLS%vsvars32.bat" x64


@mkdir %build_dir%\%configuration%
@pushd %build_dir%\%configuration%

@echo %cmake_tool% -G%generator% %source_dir% -DCMAKE_BUILD_TYPE:STRING=%configuration% %cmake_params% %cmake_params2% %8
%cmake_tool% -G%generator% %source_dir% -DCMAKE_BUILD_TYPE:STRING=%configuration% %cmake_params% %cmake_params2% %8

@popd

goto end

:error

@echo CMake tool not defined

:end
