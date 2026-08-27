/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_sdl.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:27:08 by gajanvie          #+#    #+#             */
/*   Updated: 2026/08/27 15:44:02 by gajanvie         ###   ########.fr       */
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

	vkDeviceWaitIdle(vCtx.device);

	auto recreate = [&](VulkanBuffer& buf, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props)
	{
		if (buf.buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(vCtx.device, buf.buffer, nullptr);
			vkFreeMemory(vCtx.device, buf.memory, nullptr);
			buf.buffer = VK_NULL_HANDLE;
			buf.memory = VK_NULL_HANDLE;
		}
		createBuffer(vCtx.device, vCtx.physicalDevice, size, usage, props, buf.buffer, buf.memory);
	};

	uint32_t max_rays = (uint32_t)pixel_count;

	recreate(vCtx.outputBuffer, max_rays * sizeof(uint32_t),
			 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	recreate(vCtx.accum_pixel, max_rays * 4 * sizeof(float),
			 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	recreate(vCtx.rayQueueA, max_rays * 80,
			 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	recreate(vCtx.rayQueueB, max_rays * 80,
			 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	recreate(vCtx.hitQueue, max_rays * 48,
			 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkDescriptorBufferInfo outInfo{vCtx.outputBuffer.buffer, 0, VK_WHOLE_SIZE};
	VkDescriptorBufferInfo accumInfo{vCtx.accum_pixel.buffer, 0, VK_WHOLE_SIZE};
	VkDescriptorBufferInfo hitInfo{vCtx.hitQueue.buffer, 0, VK_WHOLE_SIZE};

	for (int setIdx = 0; setIdx < 2; ++setIdx)
	{
		VkDescriptorBufferInfo rayAInfo = (setIdx == 0)
			? VkDescriptorBufferInfo{vCtx.rayQueueA.buffer, 0, VK_WHOLE_SIZE}
			: VkDescriptorBufferInfo{vCtx.rayQueueB.buffer, 0, VK_WHOLE_SIZE};
		VkDescriptorBufferInfo rayBInfo = (setIdx == 0)
			? VkDescriptorBufferInfo{vCtx.rayQueueB.buffer, 0, VK_WHOLE_SIZE}
			: VkDescriptorBufferInfo{vCtx.rayQueueA.buffer, 0, VK_WHOLE_SIZE};

		VkWriteDescriptorSet writes[5]{};
		VkDescriptorBufferInfo* infos[5] = {&outInfo, &accumInfo, &rayAInfo, &rayBInfo, &hitInfo};
		uint32_t bindings[5] = {8, 10, 11, 12, 13};

		for (int i = 0; i < 5; ++i)
		{
			writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[i].dstSet = vCtx.descriptorSets[setIdx];
			writes[i].dstBinding = bindings[i];
			writes[i].descriptorCount = 1;
			writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writes[i].pBufferInfo = infos[i];
		}
		vkUpdateDescriptorSets(vCtx.device, 5, writes, 0, nullptr);
	}
}