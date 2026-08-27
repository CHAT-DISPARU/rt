/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_init.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:21:08 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/08/27 14:58:31 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vk_context.hpp"

static uint32_t	get_compute_queue_family(VkPhysicalDevice physicalDevice)
{
	uint32_t	queueFamilyCount = 0;
	
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties>	queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			return (i);
	}
	throw std::runtime_error("pas de file dattente");
}

bool	init_vulkan(VulkanContext& vCtx)
{
	VkApplicationInfo	appInfo{};
	
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Path Tracer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Custom Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo	createInfo{};
	
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0; 
	createInfo.enabledExtensionCount = 0;
	if (vkCreateInstance(&createInfo, nullptr, &vCtx.instance) != VK_SUCCESS)
	{
		std::cerr << "create instance\n";
		return (false);
	}
	// choux du gpu
	uint32_t	deviceCount = 0;
	vkEnumeratePhysicalDevices(vCtx.instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		std::cerr << "no vulkan gpu\n";
		return (false);
	}

	std::vector<VkPhysicalDevice>	devices(deviceCount);
	vkEnumeratePhysicalDevices(vCtx.instance, &deviceCount, devices.data());
	vCtx.physicalDevice = devices[0];
	//si on trouve un vraigpu on le prend
	for (const auto& device : devices)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device, &props);
		
		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			vCtx.physicalDevice = device;
			break;
		}
	}
	VkPhysicalDeviceProperties finalProperties;
	vkGetPhysicalDeviceProperties(vCtx.physicalDevice, &finalProperties);
	std::cout << "GPU Selectionne : " << finalProperties.deviceName << "\n";

	//creation logical devise et de la queu
	vCtx.computeQueueIndex = get_compute_queue_family(vCtx.physicalDevice);

	VkDeviceQueueCreateInfo	queueCreateInfo{};
	float					queuePriority = 1.0f;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = vCtx.computeQueueIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo	deviceInfo{};
	
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.enabledExtensionCount = 0; 

	if (vkCreateDevice(vCtx.physicalDevice, &deviceInfo, nullptr, &vCtx.device) != VK_SUCCESS)
	{
		std::cerr << "create logical devic\n";
		return (false);
	}
	vkGetDeviceQueue(vCtx.device, vCtx.computeQueueIndex, 0, &vCtx.computeQueue);

	// creation de la pool de command
	VkCommandPoolCreateInfo	poolInfo{};
	
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vCtx.computeQueueIndex;
	if (vkCreateCommandPool(vCtx.device, &poolInfo, nullptr, &vCtx.commandPool) != VK_SUCCESS)
	{
		std::cerr << "create pool\n";
		return (false);
	}

	std::cout << "wtttttttttttfffffffffffffffff finalement ca marche ?\n";
	return (true);
}

void	cleanup_vulkan(VulkanContext& vCtx)
{
	if (vCtx.commandPool)
		vkDestroyCommandPool(vCtx.device, vCtx.commandPool, nullptr);
	if (vCtx.device)
		vkDestroyDevice(vCtx.device, nullptr);
	if (vCtx.instance)
		vkDestroyInstance(vCtx.instance, nullptr);
}

bool init_descriptors(VulkanContext& vCtx, int width, int height,
						const VulkanBuffer& mat_buf, const VulkanBuffer& tri_buf, const VulkanBuffer& sph_buf, 
						const VulkanBuffer& qd_buf, const VulkanBuffer& pl_buf,
						const VulkanBuffer& bvh_tri_buf, const VulkanBuffer& bvh_sph_buf, 
						const VulkanBuffer& bvh_qd_buf, const VulkanBuffer& light_buf)
{
	uint32_t max_rays = width * height;
	VkDeviceSize outputSize = max_rays * sizeof(uint32_t);
	createBuffer(vCtx.device, vCtx.physicalDevice, outputSize,
				 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 vCtx.outputBuffer.buffer, vCtx.outputBuffer.memory);

	VkDeviceSize accumSize = max_rays * 4 * sizeof(float);
	createBuffer(vCtx.device, vCtx.physicalDevice, accumSize,
				 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				 vCtx.accum_pixel.buffer, vCtx.accum_pixel.memory);

	VkDeviceSize rayStateSize = max_rays * 80;//sizeof en std 430
	VkDeviceSize hitRecordSize = max_rays * 48;
	VkDeviceSize counterSize = 16;

	createBuffer(vCtx.device, vCtx.physicalDevice, rayStateSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vCtx.rayQueueA.buffer, vCtx.rayQueueA.memory);
	createBuffer(vCtx.device, vCtx.physicalDevice, rayStateSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vCtx.rayQueueB.buffer, vCtx.rayQueueB.memory);
	createBuffer(vCtx.device, vCtx.physicalDevice, hitRecordSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vCtx.hitQueue.buffer, vCtx.hitQueue.memory);
	createBuffer(vCtx.device, vCtx.physicalDevice, counterSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vCtx.counterBuffer.buffer, vCtx.counterBuffer.memory);

	std::vector<VkDescriptorSetLayoutBinding> bindings(15);
	for (uint32_t i = 0; i < 15; i++)
	{
		bindings[i].binding = i;
		bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[i].descriptorCount = 1;
		bindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		bindings[i].pImmutableSamplers = nullptr;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(vCtx.device, &layoutInfo, nullptr, &vCtx.descriptorSetLayout) != VK_SUCCESS)
		return false;

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize.descriptorCount = 15 * 2;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 2;

	if (vkCreateDescriptorPool(vCtx.device, &poolInfo, nullptr, &vCtx.descriptorPool) != VK_SUCCESS)
		return false;

	VkDescriptorSetLayout layouts[] = {vCtx.descriptorSetLayout, vCtx.descriptorSetLayout};
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vCtx.descriptorPool;
	allocInfo.descriptorSetCount = 2;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(vCtx.device, &allocInfo, vCtx.descriptorSets) != VK_SUCCESS)
		return false;
	std::vector<VkDescriptorBufferInfo> bufferInfos(15);
	bufferInfos[0] = {mat_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[1] = {tri_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[2] = {sph_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[3] = {qd_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[4] = {pl_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[5] = {bvh_tri_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[6] = {bvh_sph_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[7] = {bvh_qd_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[8] = {vCtx.outputBuffer.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[9] = {light_buf.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[10] = {vCtx.accum_pixel.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[13] = {vCtx.hitQueue.buffer, 0, VK_WHOLE_SIZE};
	bufferInfos[14] = {vCtx.counterBuffer.buffer, 0, VK_WHOLE_SIZE};

	for (int setIdx = 0; setIdx < 2; ++setIdx)
	{
		bufferInfos[11] = (setIdx == 0) ? VkDescriptorBufferInfo{vCtx.rayQueueA.buffer, 0, VK_WHOLE_SIZE} : VkDescriptorBufferInfo{vCtx.rayQueueB.buffer, 0, VK_WHOLE_SIZE};
		bufferInfos[12] = (setIdx == 0) ? VkDescriptorBufferInfo{vCtx.rayQueueB.buffer, 0, VK_WHOLE_SIZE} : VkDescriptorBufferInfo{vCtx.rayQueueA.buffer, 0, VK_WHOLE_SIZE};

		std::vector<VkWriteDescriptorSet> descriptorWrites(15);
		for (uint32_t i = 0; i < 15; i++)
		{
			descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[i].dstSet = vCtx.descriptorSets[setIdx];
			descriptorWrites[i].dstBinding = i;
			descriptorWrites[i].dstArrayElement = 0;
			descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[i].descriptorCount = 1;
			descriptorWrites[i].pBufferInfo = &bufferInfos[i];
		}
		vkUpdateDescriptorSets(vCtx.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	std::cout << "Descripteurs Wavefront" << std::endl;
	return true;
}
