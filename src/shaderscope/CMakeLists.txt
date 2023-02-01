add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

#  Libraries

set(libs
	SDL2::SDL2-static
	SDL2::SDL2main
	bgfx::bgfx
	bgfx::bx
	imgui.cmake::imgui.cmake
  bims
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${libs})

# Copy the SDL2.dll to the same folder as the executable
# This has to be here due to add_custom_command requiring the target to be in the same scope

if (WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)
endif ()
