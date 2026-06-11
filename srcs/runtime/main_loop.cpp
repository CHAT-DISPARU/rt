/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:24:32 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/11 15:36:38 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

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
		uint8_t*	dst = (uint8_t*)texture_pixels; //uint 8 prca c des char et dcp ca fait un bit on avace de 1 bit a chaque fois
		uint8_t*	src = (uint8_t*)pixels;

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
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdl.renderer);
	SDL_RenderPresent(sdl.renderer);
}

void	main_loop(SDLContext &sdl, AppContext &app, Render &render_total)
{
	bool		running = true;
	bool		show_settings = false;
	SDL_Event	event;
	const char* res_labels[] = {"320x240", "640x480", "800x600", "1280x720", "1920x1080"};
	int			res_w[] = {320,	640, 800, 1280, 1920};
	int			res_h[] = {240, 480, 600, 720, 1080};
	int			res_count = 5;
	int			res_current = 1;
	Uint64		freq = SDL_GetPerformanceFrequency();
	Uint64		last_tick = SDL_GetPerformanceCounter();
	float		ms_per_frame = 0.0f;
	float		fps = 0.0f;
	float		cam_speed  = 0.2f;
	float		cam_rotate = 0.04f;

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (event.key.key == SDLK_ESCAPE)
					running = false;
				if (event.key.key == SDLK_H) //im gui win
					show_settings = !show_settings;
			}
		}
		Uint64	now = SDL_GetPerformanceCounter();

		ms_per_frame = (float)(now - last_tick) / (float)freq * 1000.0f;
		fps = 1000.0f / (ms_per_frame > 0.0f ? ms_per_frame : 1.0f);
		last_tick = now;
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		if (show_settings)
			show_settings_window(sdl, render_total, app, res_labels, res_w, res_h, res_count, res_current,
								fps, ms_per_frame);

		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
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
		}

		// render
		thread_calls(app.camera, render_total);
		sdl_to_screen(sdl, render_total.definitive);
	}
}
