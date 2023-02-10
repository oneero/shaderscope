# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
set(bims_headers
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims/imgui_impl_bgfx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims/imgui_impl_sdl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims/fs_ocornut_imgui.bin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims/vs_ocornut_imgui.bin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims/file-ops.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/bims/shader-utils.h")

set(bims_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bims.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shader-utils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_impl_bgfx.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_impl_sdl.cpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(bims ${bims_sources} ${bims_headers})

# We need this directory, and users of our library will need it too
target_include_directories(bims PUBLIC ./include)

# Dependencies
set(bims_libs
	SDL2::SDL2-static
	SDL2::SDL2main
	bgfx::bgfx
	bgfx::bx
	imgui.cmake::imgui.cmake
)

target_link_libraries(bims PRIVATE ${bims_libs})