// Copyright 2021 ShenMian
// License(Apache-2.0)

#pragma once

#include "VertexBuffer.h"
#include "VKBuffer.h"
#include <vulkan/vulkan.h>

class VKVertexBuffer : public VertexBuffer
{
public:
	VKVertexBuffer(const void* data, size_t size, const VertexFormat& layout, Buffer::Usage usage);

	Buffer& getBuffer() override;

	operator VkBuffer();
	operator VkBuffer() const;

private:
	VKBuffer buffer;
};
