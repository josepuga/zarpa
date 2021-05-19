cmake_minimum_required(VERSION 3.0.0)
project(zarpa VERSION 0.1.0)

# SFML
#
if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
    if (SFML_STATIC_LIBRARIES)
        include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_SFML_STATIC.cmake)
        # This is the EXACT order to work. DO NOT change or modify.
        #             -----------          -----------------------
        foreach(_SFML_DEPEND 
              #GRAPHICS ONLY freetype harfbuzz brotlidec brotlienc rpcrt4 brotlicommon png zlib bzip2 graphite
              freetype harfbuzz brotlidec brotlienc rpcrt4 brotlicommon png zlib bzip2 graphite
              opengl32 openal freetype winmm gdi32 flac vorbisenc vorbisfile vorbis ogg ws2_32 
              kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 ole32 oleaut32 uuid odbc32 odbccp32
               #https://drf-gitlab.cea.fr/partons/core/partons/blob/a47f6af70ef332bdcbfd310ea321eb13bdda9c24/cmake/modules/FindSFML.cmake
              #freetype glu32 opengl32 openal ogg flac vorbis vorbisfile vorbisenc gdi32 winmm ws2_32 winmm
                        )
            list(APPEND SFML_STATIC_DEPENDENCIES ${_SFML_DEPEND})
        endforeach(_SFML_DEPEND )
        message(STATUS "Linking SFML and dependencies statically...")
    endif()   
endif()
find_package(SFML 2.5 COMPONENTS network audio graphics window system REQUIRED) #Put AFTER set(SFML_STATIC TRUE)!!!!!


# C++ Standard
#
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
endif()
if(CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "C++ incompatible standard ${CMAKE_CXX_STANDARD}. Required C++17")
endif()


#default compiler if not set
#
if (NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CXX_COMPILER "g++")
endif()


# Compiler options
#
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /permissive-")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")# -Werror")
endif()

if (NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CXX_COMPILER "g++")
    #I prefer executable files in bin/ instead build/. Change it if you like.
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin") # build/bin/
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # bin/
endif()


# OS options
#
option(MINGW_STATIC_LINK "Link MinGW dependencies statically." ON)
option(MSVC_STATIC_LINK "Link MSVC static C++ runtime library." OFF)
if(WIN32)
    message("Windows OS detected.")
    if(MSVC AND MSVC_STATIC_LINK)
        message("Linking MSVC C++ runtime statically...")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD /MT")
    endif()
    if(NOT MSVC AND MINGW_STATIC_LINK)
        message("Linking MinGW dependencies statically...")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static -pthread")
        endif()
endif()
if(UNIX AND NOT APPLE)
    message("Linux OS detected.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()
#[[
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
]]
# Resource icon
#
set(RES_ICON_FILE "")
if(MINGW)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")
        set(RES_ICON_FILE "icon.rc")
        message(STATUS "${RES_ICON_FILE} found. Integrating icon into executable...")
        set(CMAKE_RC_COMPILER_INIT windres)
        enable_language(RC)
        set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    endif()
endif()


# Build type options. https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
#
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DEBUG TRUE)
    message(STATUS "Building Debug. Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(STATUS TRUE)
    message(STATUS "Building Release. Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

include_directories(include src src/gui src/classes)


set (simple_server
    "include/simple_server/server.cpp"
    "include/simple_server/socket.cpp"
)

#[[
add_executable(server test/server.cpp ${simple_server})
target_link_libraries(server wsock32 ws2_32)
add_executable(client test/client.cpp ${simple_server})
target_link_libraries(client wsock32 ws2_32)
]]

# Comment WIN32 to show Windows Console
#
add_executable(
    zarpa  #WIN32
    #${simple_server}
    src/main.cpp
    src/classes/error.cpp
    src/classes/ini_description.cpp
    src/classes/instructioninfo.cpp
    src/classes/location.cpp
    src/classes/message.cpp
    src/classes/resource_manager.cpp
    src/classes/script.functions.cpp
    src/classes/script.core.cpp
    src/classes/script.instructions.cpp
    src/utilities.cpp
    src/devtools.cpp
    src/parser.cpp

    src/gui/gui.core.cpp
    src/gui/gui.loaddata.cpp
    src/gui/gui.functions.cpp

    ${RES_ICON_FILE}    
)

target_link_libraries(zarpa    
    sfml-graphics sfml-audio sfml-system sfml-window   
    ${SFML_STATIC_DEPENDENCIES}
)

