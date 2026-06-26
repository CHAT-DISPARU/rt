/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:10:26 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 20:41:23 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <fstream>

struct VulkanBuffer
{
	VkBuffer		buffer;
	VkDeviceMemory	memory;
};

uint32_t	findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void		createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, 
						 VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
						 VkBuffer& buffer, VkDeviceMemory& bufferMemory);
void		copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, 
					   VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
std::vector<char>	read_file(const std::string& filename);

template <typename T>
VulkanBuffer	createAndUploadSSBO(VkDevice device, VkPhysicalDevice physicalDevice, 
									VkCommandPool commandPool, VkQueue graphicsQueue, 
									const std::vector<T>& data)
{
	VulkanBuffer	result;
	VkDeviceSize	bufferSize = sizeof(data[0]) * data.size();

	if (bufferSize == 0)
	{
		result.buffer = VK_NULL_HANDLE;
		result.memory = VK_NULL_HANDLE;
		return (result);
	}

	//creation staging pour cpu
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;
	createBuffer(device, physicalDevice, bufferSize, 
				 VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				 stagingBuffer, stagingBufferMemory);

	//copie des vec vers staging
	void*	mappedData;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &mappedData);
	memcpy(mappedData, data.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	//local gpu creation ssbo
	createBuffer(device, physicalDevice, bufferSize, 
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
				 result.buffer, result.memory);
	// envoie ducoup au ssbo
	copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, result.buffer, bufferSize);
	//clear stagging
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	return (result);
}
