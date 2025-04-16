cmake_minimum_required(VERSION 3.10)
project(TinyRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 10
    add_definitions(-DUNICODE -D_UNICODE)    # Enable Unicode
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    
    # Set Windows SDK version
    if(NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0")
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
)

# Source files
set(SOURCES
    Src/Core/Window.cpp
    Src/RHI/D3D12/gfx_dx12.cpp
    Src/Samples/Sample01/Main.cpp
)

# Create executable
add_executable(TinyRenderer_dx12 ${SOURCES})

# Link libraries
if(WIN32)
    target_link_libraries(TinyRenderer_dx12
        d3d12.lib
        dxgi.lib
        dxguid.lib
        d3dcompiler.lib
        kernel32.lib
        user32.lib
        gdi32.lib
        winspool.lib
        shell32.lib
        ole32.lib
        oleaut32.lib
        uuid.lib
        comdlg32.lib
        advapi32.lib
    )
endif() 