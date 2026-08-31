#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>


struct GPUTextureImage
{
	VkImage			image = VK_NULL_HANDLE;
	VkDeviceMemory	memory = VK_NULL_HANDLE;
	VkImageView		view = VK_NULL_HANDLE;
	uint32_t		width = 0;
	uint32_t		height = 0;
};

GPUTextureImage	createTextureImage(VkDevice device, VkPhysicalDevice physicalDevice,
									VkCommandPool commandPool, VkQueue queue,
									const uint8_t* pixelsRGBA8, uint32_t width, uint32_t height,
									bool isColorData);

VkSampler	createGlobalBindlessSampler(VkDevice device);

void	destroyTextureImage(VkDevice device, GPUTextureImage& tex);

void	writeBindlessTextures(VkDevice device, VkDescriptorSet (&descriptorSets)[2],
							const std::vector<GPUTextureImage>& textures, VkSampler sampler);
