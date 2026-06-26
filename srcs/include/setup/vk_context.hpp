/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_context.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:20:51 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 19:15:46 by CHAT-DISPAR      ###   ########.fr       */
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

struct VulkanContext
{
	VkInstance			instance = VK_NULL_HANDLE;
	VkPhysicalDevice	physicalDevice = VK_NULL_HANDLE;
	VkDevice			device = VK_NULL_HANDLE;
	VkQueue				computeQueue = VK_NULL_HANDLE;
	uint32_t			computeQueueIndex = 0;
	VkCommandPool		commandPool = VK_NULL_HANDLE;

	VulkanBuffer		outputBuffer;// buffer nouveau pixel...
	VkDescriptorSetLayout	descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool		descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet			descriptorSet = VK_NULL_HANDLE;
	VkPipelineLayout		pipelineLayout = VK_NULL_HANDLE;
	VkPipeline			computePipeline = VK_NULL_HANDLE;
};


bool	init_vulkan(VulkanContext& vCtx);
bool	init_descriptors(VulkanContext& vCtx, int width, int height,
							const VulkanBuffer& mat_buf, const VulkanBuffer& tri_buf, const VulkanBuffer& sph_buf, 
							const VulkanBuffer& qd_buf, const VulkanBuffer& pl_buf,
							const VulkanBuffer& bvh_tri_buf, const VulkanBuffer& bvh_sph_buf, 
							const VulkanBuffer& bvh_qd_buf);
void	cleanup_vulkan(VulkanContext& vCtx);
