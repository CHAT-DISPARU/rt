/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_context.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:20:51 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/08/31 09:51:29 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <array>
#include "VulkanUtils.hpp"
#include "GPUData.hpp"
#include "vk_textures.hpp"

struct VulkanContext
{
	VkInstance				instance = VK_NULL_HANDLE;
	VkPhysicalDevice		physicalDevice = VK_NULL_HANDLE;
	VkDevice				device = VK_NULL_HANDLE;
	VkQueue					computeQueue = VK_NULL_HANDLE;
	uint32_t				computeQueueIndex = 0;
	VkCommandPool			commandPool = VK_NULL_HANDLE;
	VkCommandBuffer			commandBuffers[2];
    VkFence					inFlightFences[2];
    uint32_t				currentFrame = 0;
    void*					mappedOutputBuffer = nullptr;
	VulkanBuffer			outputBuffer;// buffer nouveau pixel...
	VulkanBuffer			accum_pixel;
	VulkanBuffer			rayQueueA;
	VulkanBuffer			rayQueueB;
	VulkanBuffer			hitQueue;
	VulkanBuffer			counterBuffer;
	VulkanBuffer			indirectBuffer;
	VkDescriptorSetLayout	descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool		descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet			descriptorSets[2];
	VkPipelineLayout		pipelineLayout = VK_NULL_HANDLE;
	VkPipeline				raygenPipeline = VK_NULL_HANDLE;
	VkPipeline				intersectPipeline = VK_NULL_HANDLE;
	VkPipeline				shadePipeline = VK_NULL_HANDLE;
	VkPipeline				tonemapPipeline = VK_NULL_HANDLE;
	VkPipeline				prepareIndirectPipeline = VK_NULL_HANDLE;
};


bool	init_vulkan(VulkanContext& vCtx);
bool init_descriptors(VulkanContext& vCtx, int width, int height,
						const VulkanBuffer& mat_buf, const VulkanBuffer& tri_buf, const VulkanBuffer& sph_buf, 
						const VulkanBuffer& qd_buf, const VulkanBuffer& pl_buf,
						const VulkanBuffer& bvh_tri_buf, const VulkanBuffer& bvh_sph_buf, 
						const VulkanBuffer& bvh_qd_buf, const VulkanBuffer& light_buf,
						const std::vector<GPUTextureImage>& bindless_textures, VkSampler bindless_sampler);
void	cleanup_vulkan(VulkanContext& vCtx);
