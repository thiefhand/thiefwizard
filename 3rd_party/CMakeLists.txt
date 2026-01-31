# SDL
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(SDL)
# add_subdirectory(SDL_net)

# ImGui
add_library(imgui STATIC
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdl3.h
)
target_include_directories(imgui PUBLIC imgui/)
target_link_libraries(imgui PUBLIC SDL3::SDL3)


# Glad
add_library(glad STATIC glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)

# # LibPSL
# add_subdirectory(libpsl)

# NGHTTP2
set(ENABLE_LIB_ONLY ON)
set(ENABLE_DOC OFF)
add_subdirectory(nghttp2)

# LibCurl
set(HTTP_ONLY ON)
set(CMAKE_USE_OPENSSL OFF)
set(BUILD_CURL_EXE OFF)
set(CURL_STATICLIB ON)
set(CURL_ZLIB OFF)
set(CMAKE_USE_LIBSSH2 OFF) 
set(CURL_USE_LIBPSL OFF)
set(BUILD_EXAMPLES OFF)
add_subdirectory(libcurl)

# YamlCPP
add_subdirectory(tomlplusplus)

# LibArchive
set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
set(ENABLE_TEST OFF)
add_subdirectory(libarchive)
target_compile_definitions(thiefwizard PUBLIC -DLIBARCHIVE_STATIC)

# Link to executable
target_link_libraries(thiefwizard PUBLIC SDL3::SDL3-static)
if (MSVC)
 #  target_link_libraries(thiefwizard PUBLIC SDL3::SDL3_main)
endif()
target_link_libraries(thiefwizard PUBLIC imgui)
target_link_libraries(thiefwizard PUBLIC glad)
target_link_libraries(thiefwizard PUBLIC libcurl)
target_link_libraries(thiefwizard PUBLIC tomlplusplus::tomlplusplus)
target_link_libraries(thiefwizard PUBLIC archive_static)
