/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:25:03 by gajanvie          #+#    #+#             */
/*   Updated: 2026/07/01 14:40:47 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
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
#include "ThreadPool.hpp"
#include "ScenePacker.hpp"
#include "vk_context.hpp"
#include "VulkanUtils.hpp"

struct	SDLContext
{
	SDL_Window*		window = nullptr;
	SDL_Renderer*	renderer = nullptr;
	SDL_Texture*	texture = nullptr;
	int				width = 0;
	int				height = 0;
};

void	thread_calls(Camera &cam, Render &render_total, ThreadPool &threads);
struct	VulkanContext;
void	run_compute_frame(VulkanContext& vCtx, Camera& cam, Render& render_total, int width, int height);
void	set_gui_frame(SDLContext &sdl, Render &render_total);
void	setup_base_render(AppContext &app, Render &render_total);
void	sdl_cleanup(SDLContext &sdl);
int		sdl_init(SDLContext &sdl, int width, int height);
void	cleanup_render(Render &render_total);
void	clean_gui();
void	resize_sdl(SDLContext &sdl, int new_w, int new_h, Render &render_total, VulkanContext &vCtx);
void	show_settings_window(SDLContext &sdl, Render &render_total, AppContext &app,
			const char **res_labels, int *res_w, int *res_h, int res_count,
			int &res_current, float fps, float ms, VulkanContext &vCtx);
Vec3f	sampleTextureFast(SDL_Surface* surface, float u, float v);
float	sampleScalarLinear(SDL_Surface* surface, float u, float v, int channel);
bool	init_pipeline(VulkanContext& vCtx);
void	main_loop(SDLContext &sdl, AppContext &app, Render &render_total, VulkanContext &vCtx);

