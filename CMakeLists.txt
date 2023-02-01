cmake_minimum_required(VERSION 3.24)

option(SUPERBUILD "Perform a superbuild (or not)" OFF)

project(Shaderscope LANGUAGES CXX)

# Build third-party libs with "superbuild"

if (SUPERBUILD)
  if (EMSCRIPTEN)
    set(THIRD_PARTY_BUILD_DIR_NAME embuild)
  else ()
    set(THIRD_PARTY_BUILD_DIR_NAME build)
  endif ()
  include(third-party/CMakeLists.txt)
  include(cmake/superbuild.cmake)
  return()
endif ()

# Third-party libs must have been built at this point

find_package(SDL2 REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(bgfx REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(imgui.cmake REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)

# Libraries
add_subdirectory(src/bims)

# Applications
add_subdirectory(src/shaderscope)

# EMSCRIPTEN

target_link_options(
  ${PROJECT_NAME} PRIVATE $<$<BOOL:${EMSCRIPTEN}>:-sMAX_WEBGL_VERSION=2
  -sALLOW_MEMORY_GROWTH=1 --preload-file=shader/embuild/v_simple.bin
  --preload-file=shader/embuild/f_simple.bin>)
target_compile_definitions(
  ${PROJECT_NAME} PRIVATE $<$<BOOL:${EMSCRIPTEN}>:USE_SDL=2>)

if (EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
  add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader/embuild
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/shader/embuild/v_simple.bin
      $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader/embuild
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/shader/embuild/f_simple.bin
      $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader/embuild
    VERBATIM)
endif ()

# required for project when using visual studio generator

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})