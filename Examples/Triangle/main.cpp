﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#include "Graphics.h"

void PrintInfo();

struct Vertex
{
	Vector2 position;
	Vector3 color;
};

int main()
{
	Window::init();

	auto window = new Window("Triangle", Monitor::getPrimary().getSize() / 2);

	Renderer::setAPI(Renderer::API::OpenGL); // 设置渲染 API 为 OpenGL

	PrintInfo();

	const std::vector<Vertex> vertices = {
		{{0, 0.5}, {1, 0, 0}},
		{{0.5, -0.5}, {0, 1, 0}},
		{{-0.5, -0.5}, {0, 0, 1}}
	};

	VertexFormat format = {
		{"position", Format::RG32F},
		{"color", Format::RGB32F}
	};
	format.setStride(sizeof(Vertex));

	auto vbo = VertexBuffer::create(vertices, format);
	auto program = Program::create("Shaders/forword");
	auto cmdQueue = CommandQueue::create();
	auto cmdBuffer = CommandBuffer::create();

	bool running = true;
	window->onClose = [&]() { running = false; };
	window->onKey = [&](int action, Key key)
	{
		if(action == 1)
		{
			switch(key)
			{
			case Key::Escape:
				running = false;
				break;

			case Key::F11:
				window->setFullscreen(!window->isFullscreen());
				break;
			}
		}
	};
	window->setVisible(true); // 设置窗口可见

	while(running)
	{
		program->use();
		cmdBuffer->begin();
		{
			cmdBuffer->setViewport({0, 0}, window->getSize());
			cmdBuffer->setClearColor({0, 0, 0, 0});
			cmdBuffer->clear(ClearFlag::Color);
			cmdBuffer->setVertexBuffer(vbo);
			cmdBuffer->draw(0, vbo->getCount());
		}
		cmdBuffer->end();
		cmdQueue->submit(cmdBuffer);

		window->update();
	}
	delete window;

	Window::deinit();

	return 0;
}

void PrintInfo()
{
	// 打印显示器信息
	for(const auto& mon : Monitor::getMonitors())
	{
		/*printf(std::format(
			"Monitor\n"
			"|-Name        : {}\n"
			"|-Size        : {}x{}\n"
			"`-Refresh rate: {} Hz\n",
			mon.getName(), mon.getSize().x, mon.getSize().y, mon.getRefreshRate()).c_str());*/
		printf(
			"Monitor\n"
			"|-Name        : %s\n"
			"|-Size        : %dx%d\n"
			"`-Refresh rate: %d Hz\n",
			mon.getName().c_str(), mon.getSize().x, mon.getSize().y, mon.getRefreshRate());
	}

	// 打印基本信息
	const auto renderer = Renderer::get();
	/*printf(std::format(
		"Basic\n"
		"|-Device  : {}\n"
		"|-Renderer: {}\n"
		"`-Vendor  : {}\n",
		renderer->getDeviceName(), renderer->getRendererName(), renderer->getVendorName()).c_str());*/
	printf(
		"Renderer\n"
		"|-Device  : %s\n"
		"|-Renderer: %s\n"
		"`-Vendor  : %s\n",
		renderer->getDeviceName().c_str(),
		renderer->getRendererName().c_str(),
		renderer->getVendorName().c_str());
}
