cmake_minimum_required(VERSION 3.25...3.28)

project(RT VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
	GIT_TAG         main
	GIT_SHALLOW     TRUE
	UPDATE_DISCONNECTED TRUE
)

set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
	SDL3_image
	GIT_REPOSITORY  https://github.com/libsdl-org/SDL_image.git
	GIT_TAG         main
	GIT_SHALLOW     TRUE
	UPDATE_DISCONNECTED TRUE
)
set(SDL3IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_VENDORED ON CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3_image)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SOURCES
	srcs/main.cpp
	srcs/runtime/render.cpp
	srcs/runtime/render_utils.cpp
	srcs/runtime/Thread_calls.cpp
	srcs/runtime/main_loop.cpp
	srcs/setup/setup_base_render.cpp
	srcs/setup/setup_gui.cpp
	srcs/setup/setup_sdl.cpp
	srcs/setup/Texture.cpp
	srcs/setup/SceneLoader.cpp
	srcs/setup/stb_image_impl.cpp
	srcs/setup/ScenePacker.cpp
	srcs/setup/VulkanUtils.cpp
	srcs/setup/vk_init.cpp
	srcs/setup/vk_pipeline.cpp
	srcs/Hittables/BVHNodes.cpp
	srcs/Hittables/Plane.cpp
	srcs/Hittables/Quad.cpp
	srcs/Hittables/Sphere.cpp
	srcs/Hittables/Triangle.cpp
	srcs/Materials/Dielectric.cpp
	srcs/Materials/DiffuseLight.cpp
	srcs/Materials/Lambertian.cpp
	srcs/Materials/Metal.cpp
	srcs/Materials/PBRMaterial.cpp
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_sdl3.cpp
	imgui/backends/imgui_impl_sdlrenderer3.cpp
	imgui/imgui_demo.cpp
)

set_source_files_properties(
	srcs/setup/SceneLoader.cpp
	PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
	COMPILE_FLAGS "-Wno-error=unused-function -Wno-unused-function"
)

add_executable(rt ${SOURCES})

target_compile_options(rt PRIVATE
	-Wall -Wextra -Werror
	-O3
	-march=native
	-pipe
	-ffast-math
	-funroll-loops
	-mprefer-vector-width=256
)

# target_compile_options(rt PRIVATE
#     -Wall -Wextra -Werror
#     -g3
#     -fno-omit-frame-pointer
#     -fsanitize=address,undefined 
# )

# target_link_options(rt PRIVATE
#     -fsanitize=address,undefined
# )

target_include_directories(rt PRIVATE
	srcs/include
	srcs/include/Hittables
	srcs/include/Materials
	srcs/include/setup
	srcs/include/external
	srcs/math
	imgui
	imgui/backends
)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(rt PRIVATE 
	Threads::Threads
	SDL3::SDL3
	SDL3_image::SDL3_image
)

find_package(Vulkan REQUIRED)
target_link_libraries(rt PRIVATE Vulkan::Vulkan)
target_include_directories(rt PRIVATE ${Vulkan_INCLUDE_DIRS})


find_program(GLSLC_COMPILER glslc HINTS ${Vulkan_GLSLC_EXECUTABLE})

if(NOT GLSLC_COMPILER)
	message(FATAL_ERROR "glslc introuvable")
endif()

set(SHADER_SRC "${CMAKE_SOURCE_DIR}/srcs/shaders/raytracer.comp")
set(SHADER_OUT "${CMAKE_CURRENT_BINARY_DIR}/raytracer.spv")

add_custom_command(
	OUTPUT ${SHADER_OUT}
	COMMAND ${GLSLC_COMPILER} ${SHADER_SRC} -o ${SHADER_OUT}
	DEPENDS ${SHADER_SRC}
	COMMENT "Compiling Compute Shader: raytracer.comp -> raytracer.spv"
)

add_custom_target(Shaders ALL DEPENDS ${SHADER_OUT})
add_dependencies(rt Shaders)