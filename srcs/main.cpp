/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 16:17:25 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 16:55:24 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BVHNode.hpp"
#include <SDL3/SDL.h>
#include "Sphere.hpp"
#include "Lambertian.hpp"
#include "Math3D.hpp"
#include <chrono>
#include <iostream>
#include <iostream>
#include <chrono>
#include <vector>
#include <ctime>
#include "SceneLoader.hpp"
#include "Render.hpp"

void	thread_calls(Camera &cam, Render &render_total);

struct	SDLContext
{
	SDL_Window*		window = nullptr;
	SDL_Renderer*	renderer = nullptr;
	SDL_Texture*	texture = nullptr;
	int				width = 0;
	int				height = 0;
};

int	sdl_init(SDLContext &sdl, int width, int height)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init : " << SDL_GetError() << "\n";
		return (false);
	}

	sdl.window = SDL_CreateWindow("Ray Tracer du boss", width, height, 0);
	if (!sdl.window)
	{
		std::cerr << "create win: " << SDL_GetError() << "\n";
		return (false);
	}

	sdl.renderer = SDL_CreateRenderer(sdl.window, nullptr);
	if (!sdl.renderer)
	{
		std::cerr << "create renderer: " << SDL_GetError() << "\n";
		return (false);
	}
	//rgba
	sdl.texture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!sdl.texture)
	{
		std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
		return (false);
	}

	sdl.width = width;
	sdl.height = height;
	return (true);

}

void	sdl_to_screen(SDLContext &sdl, uint32_t *pixels)
{
	void*	texture_pixels;
	int		pitch;

	// lock la texture et recup la longeur d une ligne pitch
	if (!SDL_LockTexture(sdl.texture, nullptr, &texture_pixels, &pitch))
	{
		std::cerr << "SDL_LockTexture: " << SDL_GetError() << "\n";
		return;
	}

	// sdl fait un pading pour le gpu pour avoir 16 bit parfois donc cas parfait sinon..
	if (pitch == sdl.width * (int)sizeof(uint32_t))
		memcpy(texture_pixels, pixels, sdl.width * sdl.height * sizeof(uint32_t));
	else
	{
		uint8_t* dst = (uint8_t*)texture_pixels; //uint 8 prca c des char et dcp ca fait un bit on avace de 1 bit a chaque fois
		uint8_t* src = (uint8_t*)pixels;
		for (int y = 0; y < sdl.height; y++)
		{
			memcpy(dst, src, sdl.width * sizeof(uint32_t));
			dst += pitch;
			src += sdl.width * sizeof(uint32_t);
		}
	}
	SDL_UnlockTexture(sdl.texture);
	SDL_RenderClear(sdl.renderer);
	SDL_RenderTexture(sdl.renderer, sdl.texture, nullptr, nullptr);
	SDL_RenderPresent(sdl.renderer);
}

void	main_loop(SDLContext &sdl, AppContext &app, Render &render_total)
{
	bool		running = true;
	SDL_Event	event;
	float		cam_speed  = 0.2f;
	float		cam_rotate = 0.04f;

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
				running = false;
		}
		const bool*	keys = SDL_GetKeyboardState(nullptr);

		// deplace
		if (keys[SDL_SCANCODE_W])
			app.camera.move_forward(cam_speed);
		else if (keys[SDL_SCANCODE_S])
			app.camera.move_forward(-cam_speed);
		if (keys[SDL_SCANCODE_A])
			app.camera.move_right(-cam_speed);
		else if (keys[SDL_SCANCODE_D])
			app.camera.move_right(cam_speed);
		if (keys[SDL_SCANCODE_SPACE])
			app.camera.move_up_world(cam_speed);
		else if (keys[SDL_SCANCODE_LSHIFT])
			app.camera.move_up_world(-cam_speed);
		// rotation
		if (keys[SDL_SCANCODE_UP])
			app.camera.pitch(cam_rotate);
		else if (keys[SDL_SCANCODE_DOWN])
			app.camera.pitch(-cam_rotate);
		if (keys[SDL_SCANCODE_LEFT])
			app.camera.yaw(cam_rotate);
		else if (keys[SDL_SCANCODE_RIGHT])
			app.camera.yaw(-cam_rotate);
		if (keys[SDL_SCANCODE_LEFTBRACKET])
			app.camera.roll(cam_rotate);
		else if (keys[SDL_SCANCODE_RIGHTBRACKET])
			app.camera.roll(-cam_rotate);

		// render
		thread_calls(app.camera, render_total);

		sdl_to_screen(sdl, render_total.definitive);

		std::string title = "sample " 
						  + std::to_string(render_total.frame_count);
		SDL_SetWindowTitle(sdl.window, title.c_str());
	}
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "use: " << av[0] << " <scene_file.rt>\n";
		return (1);
	}

	AppContext	app;

	std::cout << "Loading scene file: " << av[1] << "..." << std::endl;
	if (!SceneLoader::loadRT(av[1], app))
	{
		std::cerr << "Failed to load scene." << std::endl;
		return (1);
	}

	std::cout << "Scene loaded\n";
	std::cout << "Resolution: " << app.width << "x" << app.height << "\n";
	std::cout << "Samples: " << app.samples << "\n";
	std::cout << "\n[BVH] Building..." << std::endl;
	auto	startbvh = std::chrono::high_resolution_clock::now();
	app.scene.build();
	auto	endbvh = std::chrono::high_resolution_clock::now();
	auto	durationbvh = std::chrono::duration_cast<std::chrono::milliseconds>(endbvh - startbvh);
	std::cout << "[BVH] Construction time: " << durationbvh.count() << " ms" << std::endl;
	srand(static_cast<unsigned int>(time(NULL)));

	std::cout << "start sdl init and loop" << std::endl;

	size_t		pixel_count = app.width * app.height;
	Vec3f*		accum_buffer = new Vec3f[pixel_count]();
	uint32_t*	definitive = new uint32_t[pixel_count]();

	Render		render_total(app.camera, app.scene);

	render_total.width = app.width;
	render_total.height = app.height;
	render_total.inv_w = 1.0f / (float)app.width;
	render_total.inv_h = 1.0f / (float)app.height;
	render_total.depth_max = 50;
	render_total.samples = app.samples;
	render_total.frame_count = 1;
	render_total.accum_buffer = accum_buffer;
	render_total.definitive = definitive;

	SDLContext	sdl;
	if (!sdl_init(sdl, app.width, app.height))
	{
		delete[] accum_buffer;
		delete[] definitive;
		return (1);
	}
	//boucle infinie
	
	main_loop(sdl, app, render_total);

	delete[] accum_buffer;
	delete[] definitive;
	return (0);
}