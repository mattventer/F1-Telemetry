@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
@REM Important: to build on 32-bit systems, the DX12 backends needs '#define ImTextureID ImU64', so we pass it here.
@set OUT_DIR=build
@set OUT_EXE=f123_telemetry
@set INCLUDES=/I libs\imgui /I libs\imgui\backends /I libs\implot /I libs\tinyxml2 /I "%WindowsSdkDir%Include\um" /I "%WindowsSdkDir%Include\shared"
@set SOURCES=src\main.cpp libs\imgui\backends\imgui_impl_dx12.cpp libs\imgui\backends\imgui_impl_win32.cpp libs\imgui\imgui*.cpp libs\implot\implot*.cpp libs\tinyxml2\tinyxml2*.cpp
@set LIBS=d3d12.lib d3dcompiler.lib dxgi.lib
mkdir build
cl /std:c++17 /nologo /Zi /MD /utf-8 %INCLUDES% /D ImTextureID=ImU64 /D UNICODE /D _UNICODE %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

@REM copy resources to build folder
ROBOCOPY /mir /njs /njh src\misc\fonts\ %OUT_DIR%\misc\fonts\ 
ROBOCOPY /xo /njs /njh src\storage\ %OUT_DIR%\data\ f123history.xml

@REM  Hide console: /subsystem:windows /entry:mainCRTStartup