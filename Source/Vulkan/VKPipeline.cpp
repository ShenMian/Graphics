﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#include "VKPipeline.h"
#include "../Program.h"
#include "../PipelineLayout.h"
#include <stdexcept>

namespace
{

VkDescriptorType VKType(PipelineLayout::Type type)
{
	switch(type)
	{
		using enum PipelineLayout::Type;

	case Texture:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case Sampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	};
	throw std::invalid_argument("");
}

VkShaderStageFlags VKStageFlags(int flags)
{
	VkShaderStageFlags vkFlags = {};
	if(flags & PipelineLayout::StageFlags::Vertex)
		vkFlags |= VK_SHADER_STAGE_VERTEX_BIT;
	if(flags & PipelineLayout::StageFlags::Geometry)
		vkFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if(flags & PipelineLayout::StageFlags::Fragment)
		vkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if(flags & PipelineLayout::StageFlags::Compute)
		vkFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
	return vkFlags;
}

}

VKPipeline::VKPipeline(std::shared_ptr<Program> program, const PipelineLayout& layout)
{
	auto renderer = reinterpret_cast<VKRenderer*>(Renderer::get());

	createLayout(layout);

	VkPipelineViewportStateCreateInfo viewportState = {};
	createViewportState(viewportState);

	VkPipelineRasterizationStateCreateInfo rasterizerState = {};
	createRasterizerState(rasterizerState);

	VkPipelineMultisampleStateCreateInfo multisampleState = {};

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = program->getStageCount();
	/*
	pipelineInfo.pStages = shaderStageCreateInfos;
	pipelineInfo.pVertexInputState = &vertexInputCreateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pTessellationState = (inputAssembly.topology == VK_PRIMITIVE_TOPOLOGY_PATCH_LIST ? &tessellationState : nullptr);
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizerState;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pDepthStencilState = &depthStencilState;
	pipelineInfo.pColorBlendState = &colorBlendState;
	pipelineInfo.pDynamicState = (!dynamicStatesVK.empty() ? &dynamicState : nullptr);
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = ;
	*/
	if(vkCreateGraphicsPipelines(renderer->getDevice(), nullptr, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline");
}

void VKPipeline::createLayout(const PipelineLayout& layout)
{
	auto renderer = reinterpret_cast<VKRenderer*>(Renderer::get());

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	for(const auto& attr : layout.getAttributes())
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = attr.slot;
		layoutBinding.descriptorType = VKType(attr.type);
		layoutBinding.descriptorCount = attr.arraySize;
		layoutBinding.stageFlags = VKStageFlags(attr.stageFlags);

		bindings.emplace_back(layoutBinding);
	}

	VkDescriptorSetLayout setLayout;
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	if(vkCreateDescriptorSetLayout(renderer->getDevice(), &layoutInfo, nullptr, &setLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor set layout");

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &setLayout;
	if(vkCreatePipelineLayout(renderer->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout");
}

void VKPipeline::createViewportState(VkPipelineViewportStateCreateInfo& info)
{
}

void VKPipeline::createRasterizerState(VkPipelineRasterizationStateCreateInfo&)
{
}

void VKPipeline::createMultisampleState(VkPipelineMultisampleStateCreateInfo& info)
{
}
