/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_pipeline.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:54:58 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 21:03:26 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vk_context.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>

bool	init_pipeline(VulkanContext& vCtx)
{
	std::filesystem::path	exe_dir = std::filesystem::canonical("/proc/self/exe").parent_path();
	std::filesystem::path	shader_path = exe_dir / "raytracer.spv";
	auto	shaderCode = read_file(shader_path.string());
	VkShaderModuleCreateInfo	createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
	VkShaderModule	computeShaderModule;

	if (vkCreateShaderModule(vCtx.device, &createInfo, nullptr, &computeShaderModule) != VK_SUCCESS)
	{
		std::cerr << "Shader Module\n";
		return (false);
	}

	//etape de computes
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageInfo.module = computeShaderModule;
	shaderStageInfo.pName = "main";

	//push constant
	VkPushConstantRange	pushRange{};
	pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	pushRange.offset = 0;
	pushRange.size = sizeof(GPUPushConstants);
	//pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &vCtx.descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushRange;

	if (vkCreatePipelineLayout(vCtx.device, &pipelineLayoutInfo, nullptr, &vCtx.pipelineLayout) != VK_SUCCESS)
	{
		std::cerr << "Pipeline layout\n";
		return (false);
	}

	// creation pipeline
	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = vCtx.pipelineLayout;
	pipelineInfo.stage = shaderStageInfo;

	if (vkCreateComputePipelines(vCtx.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vCtx.computePipeline) != VK_SUCCESS)
	{
		std::cerr << "Compute Pipeline\n";
		return (false);
	}

	// clear module plus bewsoin
	vkDestroyShaderModule(vCtx.device, computeShaderModule, nullptr);

	std::cout << "compute pipeline fonctionnelk" << std::endl;
	return (true);
}
