/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_sdl.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:27:08 by gajanvie          #+#    #+#             */
/*   Updated: 2026/07/01 14:39:35 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

void	sdl_cleanup(SDLContext &sdl)
{
	SDL_DestroyTexture(sdl.texture);
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
	SDL_Quit();
}

int	sdl_init(SDLContext &sdl, int width, int height)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init : " << SDL_GetError() << "\n";
		return (false);
	}
	SDL_WindowFlags	window_flags = SDL_WINDOW_RESIZABLE;
	sdl.window = SDL_CreateWindow("Ray Tracer du boss", width, height, window_flags);
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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForSDLRenderer(sdl.window, sdl.renderer);
	ImGui_ImplSDLRenderer3_Init(sdl.renderer);
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

void	resize_sdl(SDLContext &sdl, int new_w, int new_h, Render &render_total, VulkanContext &vCtx)
{
	SDL_SetWindowSize(sdl.window, new_w, new_h);
	SDL_DestroyTexture(sdl.texture);
	sdl.texture = SDL_CreateTexture(sdl.renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		new_w, new_h);
	sdl.width = new_w;
	sdl.height = new_h;

	int	pixel_count = new_w * new_h;
	delete[] render_total.accum_buffer;
	delete[] render_total.definitive;
	render_total.accum_buffer = new Vec3f[pixel_count]();
	render_total.definitive = new uint32_t[pixel_count]();
	render_total.width = new_w;
	render_total.height = new_h;
	//att gpou
	vkDeviceWaitIdle(vCtx.device);

	//free ancien
	if (vCtx.outputBuffer.buffer != VK_NULL_HANDLE) 
	{
		vkDestroyBuffer(vCtx.device, vCtx.outputBuffer.buffer, nullptr);
		vkFreeMemory(vCtx.device, vCtx.outputBuffer.memory, nullptr);
		vCtx.outputBuffer.buffer = VK_NULL_HANDLE;
		vCtx.outputBuffer.memory = VK_NULL_HANDLE;
	}

	//nouveau buff
	VkDeviceSize new_buffer_size = pixel_count * sizeof(uint32_t);
	
	createBuffer(vCtx.device, vCtx.physicalDevice, new_buffer_size,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				vCtx.outputBuffer.buffer, vCtx.outputBuffer.memory);

	//met a jour le descriptor
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = vCtx.outputBuffer.buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = vCtx.descriptorSet;
	descriptorWrite.dstBinding = 8;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	vkUpdateDescriptorSets(vCtx.device, 1, &descriptorWrite, 0, nullptr);
}
