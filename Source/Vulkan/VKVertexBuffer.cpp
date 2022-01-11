﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#include "VKVertexBuffer.h"
#include "VKRenderer.h"
#include <vector>

std::unordered_map<Format, VkFormat> VKFormat = {
	{Format::R16F, VK_FORMAT_R16_SFLOAT},
	{Format::RG16F, VK_FORMAT_R16G16_SFLOAT},
	{Format::RGB16F, VK_FORMAT_R16G16B16_SFLOAT},
	{Format::RGBA16F, VK_FORMAT_R16G16B16A16_SFLOAT},
	{Format::R32F, VK_FORMAT_R32_SFLOAT},
	{Format::RG32F, VK_FORMAT_R32G32_SFLOAT},
	{Format::RGB32F, VK_FORMAT_R32G32B32_SFLOAT},
	{Format::RGBA32F, VK_FORMAT_R32G32B32A32_SFLOAT},
};

VKVertexBuffer::VKVertexBuffer(const void* data, size_t size, uint32_t count, const VertexLayout& layout, Usage usage)
	: VertexBuffer(data, size, layout)
{
	VkVertexInputBindingDescription binding = {};
	binding.binding = 0;
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	binding.stride = layout.getStride();
	bindings.push_back(binding);

	for(const auto& attr : layout.getAttributes())
	{
		VkVertexInputAttributeDescription vkAttr = {};
		vkAttr.location = attr.location;
		vkAttr.format = VKFormat[attr.format];
		vkAttr.offset = attr.offset;
		attribs.push_back(vkAttr);
	}

	VkPipelineVertexInputStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.vertexBindingDescriptionCount = bindings.size();
	info.pVertexBindingDescriptions = bindings.data();
	info.vertexAttributeDescriptionCount = attribs.size();
	info.pVertexAttributeDescriptions = attribs.data();
}

VKVertexBuffer::~VKVertexBuffer()
{
}

void VKVertexBuffer::write(const void* data, size_t size)
{
}

void VKVertexBuffer::bind()
{
}