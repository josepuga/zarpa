#[[
MIT License

Copyright (c) 2021 José Puga

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
]]

#
# Dependencies for static link. This should work under MSYS2.
# there are 2 path. msys/mingw64/lib (default used) and msys/mingw64/x86_64-w64-mingw32/lib.
# Set the last one in MSYS2_MINGW32_LIB path.
#

set(MSYS2_MINGW32_LIB_PATH D:/dev/msys2/mingw64/x86_64-w64-mingw32/lib/)

# SFML_Graphics
find_library(FREETYPE_PATH NAMES libfreetype.a)
add_library(freetype STATIC IMPORTED)
set_property(TARGET freetype PROPERTY IMPORTED_LOCATION ${FREETYPE_PATH})

find_library(PNG_PATH NAMES libpng16.a)
add_library(png STATIC IMPORTED)
set_property(TARGET png PROPERTY IMPORTED_LOCATION ${PNG_PATH})

find_library(ZLIB_PATH NAMES libz.a)
add_library(zlib STATIC IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${ZLIB_PATH})

find_library(BZIP2_PATH NAMES libbz2.a)
add_library(bzip2 STATIC IMPORTED)
set_property(TARGET bzip2 PROPERTY IMPORTED_LOCATION ${BZIP2_PATH})

find_library(HARFBUZZ_PATH NAMES libharfbuzz.a)
add_library(harfbuzz STATIC IMPORTED)
set_property(TARGET harfbuzz PROPERTY IMPORTED_LOCATION ${HARFBUZZ_PATH})

find_library(BROTLIDEC_PATH NAMES libbrotlidec-static.a)
add_library(brotlidec STATIC IMPORTED)
set_property(TARGET brotlidec PROPERTY IMPORTED_LOCATION ${BROTLIDEC_PATH})

find_library(BROTLIENC_PATH NAMES libbrotlienc-static.a)
add_library(brotlienc STATIC IMPORTED)
set_property(TARGET brotlienc PROPERTY IMPORTED_LOCATION ${BROTLIENC_PATH})

find_library(BROTLICOMMON_PATH NAMES libbrotlicommon-static.a)
add_library(brotlicommon STATIC IMPORTED)
set_property(TARGET brotlicommon PROPERTY IMPORTED_LOCATION ${BROTLICOMMON_PATH})

find_library(GRAPHITE_PATH NAMES libgraphite2.a)
add_library(graphite STATIC IMPORTED)
set_property(TARGET graphite PROPERTY IMPORTED_LOCATION ${GRAPHITE_PATH})

find_library(RPCRT4_PATH NAMES librpcrt4.a  HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(rpcrt4 STATIC IMPORTED)
set_property(TARGET rpcrt4 PROPERTY IMPORTED_LOCATION ${RPCRT4_PATH})


# SFML_Audio
find_library(OGG_PATH NAMES libogg.a)
add_library(ogg STATIC IMPORTED)
set_property(TARGET ogg PROPERTY IMPORTED_LOCATION ${OGG_PATH})

find_library(OPENAL_PATH NAMES libopenal.a)
add_library(openal STATIC IMPORTED)
set_property(TARGET openal PROPERTY IMPORTED_LOCATION ${OPENAL_PATH})

find_library(VORBIS_PATH NAMES libvorbis.a)
add_library(vorbis STATIC IMPORTED)
set_property(TARGET vorbis PROPERTY IMPORTED_LOCATION ${VORBIS_PATH})

find_library(VORBISFILE_PATH NAMES libvorbisfile.a)
add_library(vorbisfile STATIC IMPORTED)
set_property(TARGET vorbisfile PROPERTY IMPORTED_LOCATION ${VORBISFILE_PATH})

find_library(VORBISENC_PATH NAMES libvorbisenc.a)
add_library(vorbisenc STATIC IMPORTED)
set_property(TARGET vorbisenc PROPERTY IMPORTED_LOCATION ${VORBISENC_PATH})

find_library(FLAC_PATH NAMES libFLAC.a)
add_library(flac STATIC IMPORTED)
set_property(TARGET flac PROPERTY IMPORTED_LOCATION ${FLAC_PATH}) #D:/dev/msys2/mingw64/lib/libFLAC++.a


# Not Audio but, necessary for it
find_library(WINMM_PATH NAMES libwinmm.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(winmm STATIC IMPORTED)
set_property(TARGET winmm PROPERTY IMPORTED_LOCATION ${WINMM_PATH})

find_library(GDI32_PATH NAMES libgdi32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(gdi32 STATIC IMPORTED)                   
set_property(TARGET gdi32 PROPERTY IMPORTED_LOCATION ${GDI32_PATH})

find_library(OPENGL32_PATH NAMES libopengl32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(opengl32 STATIC IMPORTED)
set_property(TARGET opengl32 PROPERTY IMPORTED_LOCATION ${OPENGL32_PATH})

find_library(WS2_32_PATH NAMES libws2_32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(ws2_32 STATIC IMPORTED)                   
set_property(TARGET ws2_32 PROPERTY IMPORTED_LOCATION ${WS2_32_PATH})

find_library(GLU32_PATH NAMES libglu32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(glu32 STATIC IMPORTED)                   
set_property(TARGET glu32 PROPERTY IMPORTED_LOCATION ${GLU32_PATH})

find_library(GLEW32_PATH NAMES libglew32.a)
add_library(glew32 STATIC IMPORTED)                   
set_property(TARGET glew32 PROPERTY IMPORTED_LOCATION ${GLEW32_PATH})

find_library(WINSPOOL_PATH NAMES libwinspool.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(winspool STATIC IMPORTED)                   
set_property(TARGET winspool PROPERTY IMPORTED_LOCATION ${WINSPOOL_PATH})

find_library(UUID_PATH NAMES libuuid.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(uuid STATIC IMPORTED)                   
set_property(TARGET uuid PROPERTY IMPORTED_LOCATION ${UUID_PATH})

find_library(KERNEL32_PATH NAMES libkernel32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(kernel32 STATIC IMPORTED)                   
set_property(TARGET kernel32 PROPERTY IMPORTED_LOCATION ${KERNEL32_PATH})

find_library(USER32_PATH NAMES libuser32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(user32 STATIC IMPORTED)                   
set_property(TARGET user32 PROPERTY IMPORTED_LOCATION ${USER32_PATH})

find_library(COMDLG32_PATH NAMES libcomdlg32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(comdlg32 STATIC IMPORTED)                   
set_property(TARGET comdlg32 PROPERTY IMPORTED_LOCATION ${COMDLG32_PATH})

find_library(ADVAPI32_PATH NAMES libadvapi32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(advapi32 STATIC IMPORTED)                   
set_property(TARGET advapi32 PROPERTY IMPORTED_LOCATION ${ADVAPI32_PATH})

find_library(OLEAUT32_PATH NAMES liboleaut32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(oleaut32 STATIC IMPORTED)                   
set_property(TARGET oleaut32 PROPERTY IMPORTED_LOCATION ${OLEAUT32_PATH})

find_library(ODBC32_PATH NAMES libodbc32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(odbc32 STATIC IMPORTED)                   
set_property(TARGET odbc32 PROPERTY IMPORTED_LOCATION ${ODBC32_PATH})

find_library(ODBCCP32_PATH NAMES libodbc32.a HINTS ${MSYS2_MINGW32_LIB_PATH})
add_library(odbccp32 STATIC IMPORTED)                   
set_property(TARGET odbccp32 PROPERTY IMPORTED_LOCATION ${ODBCCP32_PATH})
