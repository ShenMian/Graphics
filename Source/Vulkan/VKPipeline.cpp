﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#include "VKPipeline.h"
#include "VKProgram.h"
#include "VKVertexBuffer.h"
#include "../PipelineLayout.h"
#include <unordered_map>
#include <stdexcept>

namespace
{

std::unordered_map<PolygonMode, VkPolygonMode> VKPolygonMode = {
	{PolygonMode::Fill, VK_POLYGON_MODE_FILL},
	{PolygonMode::Wireframe, VK_POLYGON_MODE_LINE}
};

std::unordered_map<CullMode, VkCullModeFlags> VKCullMode = {
	{CullMode::Disabled, VK_CULL_MODE_NONE},
	{CullMode::Front, VK_CULL_MODE_FRONT_BIT},
	{CullMode::Back, VK_CULL_MODE_BACK_BIT},
};

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
	VkShaderStageFlags vkFlags = 0;
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

VKPipeline::VKPipeline(const Descriptor& desc)
	: Pipeline(desc)
{
	auto renderer = reinterpret_cast<VKRenderer*>(Renderer::get());

	auto vertexBuffer = std::dynamic_pointer_cast<VKVertexBuffer>(desc.vertexBuffer);
	auto program = std::dynamic_pointer_cast<VKProgram>(desc.program);

	createLayout(desc);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
	createInputAssemblyState(inputAssemblyState, desc);

	std::vector<VkViewport> vkViewports;
	std::vector<VkRect2D> vkScissors(1);
	VkPipelineViewportStateCreateInfo viewportState = {};
	createViewportState(viewportState, desc, vkViewports, vkScissors);

	VkPipelineRasterizationStateCreateInfo rasterizerState = {};
	createRasterizerState(rasterizerState, desc);

	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	createMultisampleState(multisampleState, desc);

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	createDepthStencilState(depthStencilState, desc);

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = {};
	VkPipelineColorBlendStateCreateInfo colorBlendState = {};
	createColorBlendState(colorBlendState, desc, colorBlendAttachments);

	// 创建 Render Pass
	std::vector<VkAttachmentDescription> colorAttachments;
	std::vector<VkSubpassDescription> subpasses;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colorAttachments.push_back(colorAttachment);

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses.push_back(subpass);

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(colorAttachments.size());
	renderPassInfo.pAttachments = colorAttachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();

	VkRenderPass renderPass;
	if(vkCreateRenderPass(renderer->getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass");

	auto vertexInputState = vertexBuffer->getInfo();

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = program->getStageCount();
	pipelineInfo.pStages = program->getInfos().data();
	pipelineInfo.pVertexInputState = &vertexInputState;
	pipelineInfo.pInputAssemblyState = &inputAssemblyState;
	// pipelineInfo.pTessellationState = (inputAssembly.topology == VK_PRIMITIVE_TOPOLOGY_PATCH_LIST ? &tessellationState : nullptr);
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizerState;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pDepthStencilState = &depthStencilState;
	// pipelineInfo.pColorBlendState = &colorBlendState;
	// pipelineInfo.pDynamicState = (!dynamicStatesVK.empty() ? &dynamicState : nullptr);
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	if(vkCreateGraphicsPipelines(renderer->getDevice(), nullptr, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline");
}

VkPipeline& VKPipeline::getNativeHandle()
{
	return pipeline;
}

void VKPipeline::createLayout(const Descriptor& desc)
{
	auto renderer = reinterpret_cast<VKRenderer*>(Renderer::get());

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	for(const auto& attr : desc.layout.getAttributes())
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

void VKPipeline::createInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& info, const Descriptor& desc)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	info.primitiveRestartEnable = VK_FALSE;
}

void VKPipeline::createViewportState(VkPipelineViewportStateCreateInfo& info, const Descriptor& desc, std::vector<VkViewport>& vkViewports, std::vector<VkRect2D>& vkScissors)
{
	for(const auto& viewport : desc.viewports)
	{
		VkViewport vkViewport;
		vkViewport.x = viewport.x;
		vkViewport.y = viewport.y;
		vkViewport.width = viewport.width;
		vkViewport.height = viewport.height;
		vkViewport.minDepth = viewport.minDepth;
		vkViewport.maxDepth = viewport.maxDepth;
		vkViewports.push_back(vkViewport);
	}

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	info.viewportCount = static_cast<uint32_t>(vkViewports.size());
	info.pViewports = vkViewports.data();
	info.scissorCount = static_cast<uint32_t>(vkScissors.size());
	info.pScissors = vkScissors.data();
}

void VKPipeline::createRasterizerState(VkPipelineRasterizationStateCreateInfo& info, const Descriptor& desc)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.depthClampEnable = VK_FALSE;
	info.rasterizerDiscardEnable = VK_FALSE;
	info.lineWidth = 1.0f;
	info.polygonMode = VKPolygonMode[desc.rasterizer.polygonMode];
	info.cullMode = VKCullMode[desc.rasterizer.cullMode];
	info.depthBiasEnable = VK_FALSE;
	info.depthBiasConstantFactor = 0.0f; // Optional
	info.depthBiasClamp = 0.0f; // Optional
	info.depthBiasSlopeFactor = 0.0f; // Optional
}

void VKPipeline::createMultisampleState(VkPipelineMultisampleStateCreateInfo& info, const Descriptor& desc)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.sampleShadingEnable = VK_FALSE;
	info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	info.minSampleShading = 1.0f; // Optional
	info.pSampleMask = nullptr; // Optional
	info.alphaToCoverageEnable = VK_FALSE; // Optional
	info.alphaToOneEnable = VK_FALSE; // Optional
}

void VKPipeline::createDepthStencilState(VkPipelineDepthStencilStateCreateInfo& info, const Descriptor& desc)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
}

void VKPipeline::createColorBlendState(VkPipelineColorBlendStateCreateInfo& info, const Descriptor& desc, std::vector<VkPipelineColorBlendAttachmentState>& attachments)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.logicOpEnable = VK_FALSE;
	info.logicOp = VK_LOGIC_OP_COPY; // Optional
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.blendConstants[0] = 0.0f; // Optional
	info.blendConstants[1] = 0.0f; // Optional
	info.blendConstants[2] = 0.0f; // Optional
	info.blendConstants[3] = 0.0f; // Optional
}
