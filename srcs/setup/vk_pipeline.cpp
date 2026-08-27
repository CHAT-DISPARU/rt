/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_pipeline.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:54:58 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/08/27 14:54:33 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vk_context.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <array>

bool init_pipeline(VulkanContext& vCtx)
{
	std::filesystem::path exe_dir = std::filesystem::canonical("/proc/self/exe").parent_path();

	auto createShaderModule = [&](const std::string& filename) -> VkShaderModule {
		auto shaderCode = read_file((exe_dir / filename).string());
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
		
		VkShaderModule module;
		if (vkCreateShaderModule(vCtx.device, &createInfo, nullptr, &module) != VK_SUCCESS)
			throw std::runtime_error("error module shader: " + filename);
		return module;
	};

	VkShaderModule raygenModule;
	VkShaderModule intersectModule;
	VkShaderModule shadeModule;
	VkShaderModule tonemapModule;

	try {
		raygenModule    = createShaderModule("raygen.spv");
		intersectModule = createShaderModule("intersect.spv");
		shadeModule     = createShaderModule("shade.spv");
		tonemapModule   = createShaderModule("tonemap.spv");
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return false;
	}

	//config push constanbt
	VkPushConstantRange pushRange{};
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
		std::cerr << "error Pipeline layout\n";
		return false;
	}
	std::array<VkComputePipelineCreateInfo, 4> pipelineInfos{};
	std::array<VkShaderModule, 4> modules = {raygenModule, intersectModule, shadeModule, tonemapModule};
	
	for (int i = 0; i < 4; ++i)
	{
		pipelineInfos[i].sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfos[i].layout = vCtx.pipelineLayout;
		pipelineInfos[i].stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineInfos[i].stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		pipelineInfos[i].stage.module = modules[i];
		pipelineInfos[i].stage.pName = "main";
	}

	std::array<VkPipeline, 4> pipelines{};
	if (vkCreateComputePipelines(vCtx.device, VK_NULL_HANDLE, 4, pipelineInfos.data(), nullptr, pipelines.data()) != VK_SUCCESS)
	{
		std::cerr << "error Compute Pipelines\n";
		return false;
	}

	vCtx.raygenPipeline = pipelines[0];
	vCtx.intersectPipeline = pipelines[1];
	vCtx.shadePipeline = pipelines[2];
	vCtx.tonemapPipeline = pipelines[3];

	for (auto module : modules)
		vkDestroyShaderModule(vCtx.device, module, nullptr);

	std::cout << "wavefront pipeline op" << std::endl;
	return true;
}