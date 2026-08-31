#include "vk_textures.hpp"
#include <stdexcept>
#include <cstring>


static uint32_t findMemTypeLocal(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags props)
{
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
	for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
	{
		if ((typeFilter & (1u << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props)
			return i;
	}
	throw std::runtime_error("vk_textures: aucun type de memoire compatible");
}

static VkCommandBuffer beginSingleTimeCmdLocal(VkDevice device, VkCommandPool pool)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmd;
	vkAllocateCommandBuffers(device, &allocInfo, &cmd);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd, &beginInfo);
	return cmd;
}

static void endSingleTimeCmdLocal(VkDevice device, VkCommandPool pool, VkQueue queue, VkCommandBuffer cmd)
{
	vkEndCommandBuffer(cmd);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, pool, 1, &cmd);
}

static void transitionImageLayoutLocal(VkCommandBuffer cmd, VkImage image,
										VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags srcStage, dstStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}
	else
		throw std::runtime_error("vk_textures: transition de layout non geree");

	vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

GPUTextureImage createTextureImage(VkDevice device, VkPhysicalDevice physicalDevice,
									VkCommandPool commandPool, VkQueue queue,
									const uint8_t* pixelsRGBA8, uint32_t width, uint32_t height,
									bool isColorData)
{
	(void)isColorData;

	VkDeviceSize imageSize = (VkDeviceSize)width * height * 4;
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

	// ---- 1. staging buffer ----
	VkBuffer       stagingBuffer;
	VkDeviceMemory stagingMemory;

	VkBufferCreateInfo bufInfo{};
	bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufInfo.size  = imageSize;
	bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(device, &bufInfo, nullptr, &stagingBuffer);

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, stagingBuffer, &memReq);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemTypeLocal(physicalDevice, memReq.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory);
	vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);

	void* mapped;
	vkMapMemory(device, stagingMemory, 0, imageSize, 0, &mapped);
	memcpy(mapped, pixelsRGBA8, (size_t)imageSize);
	vkUnmapMemory(device, stagingMemory);

	GPUTextureImage tex{};
	tex.width = width;
	tex.height = height;

	VkImageCreateInfo imgInfo{};
	imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgInfo.imageType = VK_IMAGE_TYPE_2D;
	imgInfo.extent = { width, height, 1 };
	imgInfo.mipLevels = 1;
	imgInfo.arrayLayers = 1;
	imgInfo.format = format;
	imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkCreateImage(device, &imgInfo, nullptr, &tex.image);

	VkMemoryRequirements imgMemReq;
	vkGetImageMemoryRequirements(device, tex.image, &imgMemReq);

	VkMemoryAllocateInfo imgAllocInfo{};
	imgAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imgAllocInfo.allocationSize = imgMemReq.size;
	imgAllocInfo.memoryTypeIndex = findMemTypeLocal(physicalDevice, imgMemReq.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkAllocateMemory(device, &imgAllocInfo, nullptr, &tex.memory);
	vkBindImageMemory(device, tex.image, tex.memory, 0);

	VkCommandBuffer cmd = beginSingleTimeCmdLocal(device, commandPool);

	transitionImageLayoutLocal(cmd, tex.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageExtent = { width, height, 1 };
	vkCmdCopyBufferToImage(cmd, stagingBuffer, tex.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	transitionImageLayoutLocal(cmd, tex.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	endSingleTimeCmdLocal(device, commandPool, queue, cmd);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = tex.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	vkCreateImageView(device, &viewInfo, nullptr, &tex.view);

	return tex;
}

VkSampler createGlobalBindlessSampler(VkDevice device)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;


	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	VkSampler sampler;
	if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("createGlobalBindlessSampler: echec vkCreateSampler");
	return sampler;
}

void destroyTextureImage(VkDevice device, GPUTextureImage& tex)
{
	if (tex.view)   vkDestroyImageView(device, tex.view, nullptr);
	if (tex.image)  vkDestroyImage(device, tex.image, nullptr);
	if (tex.memory) vkFreeMemory(device, tex.memory, nullptr);
	tex = {};
}

void writeBindlessTextures(VkDevice device, VkDescriptorSet (&descriptorSets)[2],
							const std::vector<GPUTextureImage>& textures, VkSampler sampler)
{
	if (textures.empty())
		return;

	std::vector<VkDescriptorImageInfo> imageInfos(textures.size());
	for (size_t i = 0; i < textures.size(); i++)
	{
		imageInfos[i].sampler     = sampler;
		imageInfos[i].imageView   = textures[i].view;
		imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	for (int s = 0; s < 2; s++)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSets[s];
		write.dstBinding = 16;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = (uint32_t)imageInfos.size();
		write.pImageInfo = imageInfos.data();
		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	}
}
