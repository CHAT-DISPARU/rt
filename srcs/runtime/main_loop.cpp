/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:24:32 by gajanvie          #+#    #+#             */
/*   Updated: 2026/07/09 12:39:37 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

void	run_compute_frame(VulkanContext& vCtx, Camera& cam, Render& render_total)
{
	// allopaue et lance la cmd
	VkCommandBufferAllocateInfo	allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vCtx.commandPool;
	allocInfo.commandBufferCount = 1;
	VkCommandBuffer	cmd;
	vkAllocateCommandBuffers(vCtx.device, &allocInfo, &cmd);

	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd, &beginInfo);

	// lancer les shaders
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, vCtx.computePipeline);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, vCtx.pipelineLayout, 0, 1, &vCtx.descriptorSet, 0, nullptr);

	GPUPushConstants	pc{};

	pc.cam_origin = cam.getOrigin();
	pc.cam_forward = cam.getDir();
	pc.cam_right = cam.getRight();
	pc.cam_up = cam.getUp();
	pc.fov = cam.getFov();
	pc.frame_count = render_total.frame_count;
	pc.max_depth = render_total.depth_max;
	pc.seed = (uint32_t)rand();
	pc.time = (float)SDL_GetTicks() / 1000.0f;
	pc.w_h = render_total.height;
	pc.w_w = render_total.width;
	pc.ru_enabled = render_total.ru_enabled;
	pc.shadow_ray = render_total.shadow_ray;
	pc.light_count = render_total.scene.getLightsCount();
	
	//on envoiue au gpu
	vkCmdPushConstants(cmd, vCtx.pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(GPUPushConstants), &pc);
	//plaque de 16x16
	uint32_t	groupCountX = (render_total.width + 15) / 16;
	uint32_t	groupCountY = (render_total.height + 15) / 16;

	vkCmdDispatch(cmd, groupCountX, groupCountY, 1);
	vkEndCommandBuffer(cmd);

	// att le gpu
	VkSubmitInfo	submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;

	vkQueueSubmit(vCtx.computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vCtx.computeQueue);
	vkFreeCommandBuffers(vCtx.device, vCtx.commandPool, 1, &cmd);
	void*			mappedData;
	VkDeviceSize	outputSize = render_total.width * render_total.height * sizeof(uint32_t); //RGBA 8 bite
	vkMapMemory(vCtx.device, vCtx.outputBuffer.memory, 0, outputSize, 0, &mappedData);
	memcpy(render_total.definitive, mappedData, outputSize);
	vkUnmapMemory(vCtx.device, vCtx.outputBuffer.memory);
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

void	main_loop(SDLContext &sdl, AppContext &app, Render &render_total, VulkanContext &vCtx, ThreadPool &threads)
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
								fps, ms_per_frame, vCtx);

		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			const bool*	keys = SDL_GetKeyboardState(nullptr);

			// deplace
			if (keys[SDL_SCANCODE_W])
				app.camera.move_forward(render_total.cam_speed);
			else if (keys[SDL_SCANCODE_S])
				app.camera.move_forward(-render_total.cam_speed);
			if (keys[SDL_SCANCODE_A])
				app.camera.move_right(-render_total.cam_speed);
			else if (keys[SDL_SCANCODE_D])
				app.camera.move_right(render_total.cam_speed);
			if (keys[SDL_SCANCODE_SPACE])
				app.camera.move_up_world(render_total.cam_speed);
			else if (keys[SDL_SCANCODE_LSHIFT])
				app.camera.move_up_world(-render_total.cam_speed);
			// rotation
			if (keys[SDL_SCANCODE_UP])
				app.camera.pitch(render_total.cam_rotate);
			else if (keys[SDL_SCANCODE_DOWN])
				app.camera.pitch(-render_total.cam_rotate);
			if (keys[SDL_SCANCODE_LEFT])
				app.camera.yaw(render_total.cam_rotate);
			else if (keys[SDL_SCANCODE_RIGHT])
				app.camera.yaw(-render_total.cam_rotate);
			if (keys[SDL_SCANCODE_LEFTBRACKET])
				app.camera.roll(render_total.cam_rotate);
			else if (keys[SDL_SCANCODE_RIGHTBRACKET])
				app.camera.roll(-render_total.cam_rotate);
		}
		//thread_calls(app.camera, render_total, threads);
		//sdl_to_screen(sdl, render_total.definitive);
		//render
		(void)threads;
		if (app.camera.hasMoved())
		{
			render_total.frame_count = 1;
			app.camera.resetMovedFlag();
		}
		if (app.samples == -1 || render_total.frame_count <= (size_t)app.samples)
		{
			run_compute_frame(vCtx, render_total.cam, render_total);
			sdl_to_screen(sdl, render_total.definitive);
			render_total.frame_count++;
		}
	}
}
