// Copyright 2021 ShenMian
// License(Apache-2.0)

#pragma once

#include "IndexBuffer.h"
#include "GLBuffer.h"
#include <glad/glad.h>

class GLIndexBuffer : public IndexBuffer
{
public:
	GLIndexBuffer(const uint32_t* data, size_t size, Buffer::Usage usage);

	Buffer& getBuffer() override;

	void bind();

private:
	GLBuffer buffer;
};
