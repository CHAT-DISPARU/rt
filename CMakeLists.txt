cmake_minimum_required(VERSION 3.25...3.28)

project(RT VERSION 1.0 LANGUAGES CXX)

include(FetchContent)


FetchContent_Declare(
	SDL3
	GIT_REPOSITORY	https://github.com/libsdl-org/SDL.git
	GIT_TAG			main
)

set(SDL_X11_XTEST OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SOURCES
	srcs/main.cpp
	srcs/render.cpp
	srcs/Thread_calls.cpp
	srcs/Hittables/BVHNodes.cpp
	srcs/Hittables/Plane.cpp
	srcs/Hittables/Sphere.cpp
	srcs/Hittables/Triangle.cpp
	srcs/Materials/Dielectric.cpp
	srcs/Materials/DiffuseLight.cpp
	srcs/Materials/Lambertian.cpp
	srcs/Materials/Metal.cpp
)


add_executable(rt ${SOURCES})

target_compile_options(rt PRIVATE
	-Wall -Wextra -Werror
	-O3
	-march=native
	-flto
	-pipe
	-g -fsanitize=address
)

target_include_directories(rt PRIVATE
	srcs/include
	srcs/include/Hittables
	srcs/include/Materials
	srcs/math
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(rt PRIVATE Threads::Threads)

target_link_libraries(rt PRIVATE SDL3::SDL3)